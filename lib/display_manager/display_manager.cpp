/**
 * @file display_manager.cpp
 * @brief Implementation of display and touch management functionality
 * @note  Ported from LVGL v8 driver API (lv_disp_drv_t / lv_indev_drv_t) to the
 *        LVGL v9 display/indev API (lv_display_t / lv_indev_t).
 */

 #include "display_manager.h"
 #include <stdio.h>
 #include "driver/gpio.h"
 #include "driver/i2c.h"
 #include "driver/spi_master.h"
 #include "esp_timer.h"
 #include "esp_lcd_panel_vendor.h"
 #include "esp_err.h"
 #include "esp_log.h"
 #include "esp_lcd_sh8601.h"
 #include "touch_bsp.h"
 
 static const char *TAG = "display_mgr";
 static SemaphoreHandle_t lvgl_mux = NULL;

 // Kept at file scope so display_set_rotation() can update both LVGL and the
 // physical panel after display_init() has run (v9 no longer has a
 // drv_update_cb hook that LVGL calls automatically on rotation change).
 static lv_display_t *s_disp = NULL;
 static esp_lcd_panel_handle_t s_panel_handle = NULL;
 
 // LCD initialization commands
 static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
   {0x11, (uint8_t []){0x00}, 0, 120},   
   {0x36, (uint8_t []){0xF0}, 1, 0},   
   {0x3A, (uint8_t []){0x55}, 1, 0},  //16bits-RGB565
   {0x2A, (uint8_t []){0x00,0x00,0x02,0x17}, 4, 0}, 
   {0x2B, (uint8_t []){0x00,0x00,0x00,0xEF}, 4, 0},
   {0x51, (uint8_t []){0x00}, 1, 10},
   {0x29, (uint8_t []){0x00}, 0, 10},
   {0x51, (uint8_t []){0xFF}, 1, 0},
 };
 
 // Forward declarations of internal functions
 static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
 static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
 static void apply_panel_rotation(esp_lcd_panel_handle_t panel_handle, lv_display_rotation_t rotation);
 static void lvgl_rounder_event_cb(lv_event_t *e);
 static void increase_lvgl_tick(void *arg);
 static void lvgl_port_task(void *arg);
 
 #if EXAMPLE_USE_TOUCH
 static void lvgl_touch_cb(lv_indev_t *indev, lv_indev_data_t *data);
 #endif
 
 // Notifies LVGL that the flush is ready
 static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
 {
   lv_display_t *disp = (lv_display_t *)user_ctx;
   lv_display_flush_ready(disp);
   return false;
 }
 
 // LVGL flush callback
 // NOTE: v9 passes a raw uint8_t* pixel buffer (px_map) instead of lv_color_t*.
 // With LV_COLOR_DEPTH == 16 this buffer holds the same RGB565 bytes as before,
 // so it can be handed straight to esp_lcd_panel_draw_bitmap().
 static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
 {
   esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) lv_display_get_user_data(disp);
   const int offsetx1 = area->x1;
   const int offsetx2 = area->x2;
   const int offsety1 = area->y1;
   const int offsety2 = area->y2;
 
   // copy a buffer's content to a specific area of the display
   esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
 }
 
 // Applies the panel-level swap/mirror settings for a given LVGL rotation.
 // In v8 this ran automatically via disp_drv.drv_update_cb whenever LVGL's
 // rotation changed. v9 dropped that hook, so it is now called explicitly:
 // once at init, and again from display_set_rotation() if you rotate at runtime.
 //
 // NOTE: this SH8601 panel logs "swap_xy is not supported by this panel" and
 // returns an error from esp_lcd_panel_swap_xy() — it's a fixed-orientation
 // QSPI/round panel that only supports mirroring, not axis swap. The calls
 // below are trimmed to mirror-only to stop the (harmless but noisy) error
 // log; if you later confirm swap_xy IS needed for your exact module, restore
 // it but check/log the returned esp_err_t instead of ignoring it.
 static void apply_panel_rotation(esp_lcd_panel_handle_t panel_handle, lv_display_rotation_t rotation)
 {
   switch (rotation)
   {
     case LV_DISPLAY_ROTATION_0:
       esp_lcd_panel_mirror(panel_handle, true, false);
       break;
     case LV_DISPLAY_ROTATION_90:
       esp_lcd_panel_mirror(panel_handle, true, true);
       break;
     case LV_DISPLAY_ROTATION_180:
       esp_lcd_panel_mirror(panel_handle, false, true);
       break;
     case LV_DISPLAY_ROTATION_270:
       esp_lcd_panel_mirror(panel_handle, false, false);
       break;
   }
 }
 
 // LVGL rounder callback - ensures coordinates are properly aligned
 // NOTE: v9 removed the disp_drv.rounder_cb field entirely. The documented
 // replacement is an event callback on LV_EVENT_INVALIDATE_AREA, registered
 // via lv_display_add_event_cb() instead of a dedicated setter function.
 // Caveat (see lvgl/lvgl#8582): this only rounds the *flush* area, not the
 // area LVGL actually renders into, so the extra rounded pixel(s) can contain
 // stale/garbage data rather than freshly drawn pixels. That's usually
 // harmless for a 1px expand-to-even like this one, but if you see fringing
 // artifacts on partial updates, force a full-screen redraw (e.g.
 // lv_obj_invalidate(lv_screen_active())) after layout changes.
 static void lvgl_rounder_event_cb(lv_event_t *e)
 {
   lv_area_t *area = (lv_area_t *)lv_event_get_invalidated_area(e);

   uint16_t x1 = area->x1;
   uint16_t x2 = area->x2;
 
   uint16_t y1 = area->y1;
   uint16_t y2 = area->y2;
 
   // round the start of coordinate down to the nearest 2M number
   area->x1 = (x1 >> 1) << 1;
   area->y1 = (y1 >> 1) << 1;
   // round the end of coordinate up to the nearest 2N+1 number
   area->x2 = ((x2 >> 1) << 1) + 1;
   area->y2 = ((y2 >> 1) << 1) + 1;
 }
 
 #if EXAMPLE_USE_TOUCH
 // LVGL touch callback
 static void lvgl_touch_cb(lv_indev_t *indev, lv_indev_data_t *data)
 {
   uint16_t tp_x, tp_y;
   uint8_t win;
   win = getTouch(&tp_x, &tp_y);
   if (win) 
   {
     data->point.x = tp_x;
     data->point.y = tp_y;
     data->state = LV_INDEV_STATE_PRESSED;
   } 
   else 
   {
     data->state = LV_INDEV_STATE_RELEASED;
   }
 }
 #endif
 
 // Increases LVGL tick counter (for timing)
 static void increase_lvgl_tick(void *arg)
 {
   /* Tell LVGL how many milliseconds has elapsed */
   lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
 }
 
 // LVGL task handler
 static void lvgl_port_task(void *arg)
 {
   ESP_LOGI(TAG, "Starting LVGL task");
   uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
   while (1)
   {
     // Lock the mutex due to the LVGL APIs are not thread-safe
     if (display_lvgl_lock(-1)) {
         task_delay_ms = lv_timer_handler();
         // Release the mutex
         display_lvgl_unlock();
     }
     if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
         task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
     } else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
         task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
     }
     vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
   }
 }
 
 // Public functions implementation
 
 esp_err_t display_init()
 {
   esp_err_t ret = ESP_OK;
 
 #if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
     ESP_LOGI(TAG, "Configure backlight GPIO");
     gpio_config_t bk_gpio_config = {
         .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT,
         .mode = GPIO_MODE_OUTPUT,
     };
     ret = gpio_config(&bk_gpio_config);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "Failed to configure backlight GPIO");
         return ret;
     }
 #endif
 
   ESP_LOGI(TAG, "Initialize LVGL library");
   lv_init();
 
   // Create the LVGL display object early: its pointer is needed below as the
   // user_ctx for the panel IO "flush ready" callback, before the panel itself
   // exists. This is a v9-specific reordering versus the old disp_drv struct,
   // whose address could be taken before it was fully populated.
   lv_display_t *disp = lv_display_create(EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
   if (disp == NULL) {
       ESP_LOGE(TAG, "Failed to create LVGL display");
       return ESP_ERR_NO_MEM;
   }
   s_disp = disp;
 
   ESP_LOGI(TAG, "Initialize SPI bus");
   const spi_bus_config_t buscfg = SH8601_PANEL_BUS_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_PCLK,
                                                                EXAMPLE_PIN_NUM_LCD_DATA0,
                                                                EXAMPLE_PIN_NUM_LCD_DATA1,
                                                                EXAMPLE_PIN_NUM_LCD_DATA2,
                                                                EXAMPLE_PIN_NUM_LCD_DATA3,
                                                                EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8);
   ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to initialize SPI bus");
       return ret;
   }
 
   ESP_LOGI(TAG, "Install panel IO");
   esp_lcd_panel_io_handle_t io_handle = NULL;
   const esp_lcd_panel_io_spi_config_t io_config = SH8601_PANEL_IO_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_CS,
                                                                                 notify_lvgl_flush_ready,
                                                                                 disp);
   sh8601_vendor_config_t vendor_config = {
       .init_cmds = lcd_init_cmds,
       .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
       .flags = {
           .use_qspi_interface = 1,
       },
   };
   
   // Attach the LCD to the SPI bus
   ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to create panel IO");
       return ret;
   }
 
   esp_lcd_panel_handle_t panel_handle = NULL;
   const esp_lcd_panel_dev_config_t panel_config = {
       .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
       .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
       .bits_per_pixel = LCD_BIT_PER_PIXEL,
       .vendor_config = &vendor_config,
   };
   
   ESP_LOGI(TAG, "Install SH8601 panel driver");
   ret = esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to create panel");
       return ret;
   }
   
   ret = esp_lcd_panel_reset(panel_handle);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to reset panel");
       return ret;
   }
   
   ret = esp_lcd_panel_init(panel_handle);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to initialize panel");
       return ret;
   }
   
   // User can flush pre-defined pattern to the screen before turning on the screen or backlight
   ret = esp_lcd_panel_disp_on_off(panel_handle, true);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to turn on display");
       return ret;
   }
 
   s_panel_handle = panel_handle;
 
 #if EXAMPLE_USE_TOUCH
   ESP_LOGI(TAG, "Initialize touch controller");
   Touch_Init();
 #endif
 
    // Alloc draw buffers used by LVGL
    // It's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = (lv_color_t*)heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    if (buf1 == NULL) {
        ESP_LOGE(TAG, "Failed to allocate LVGL buffer 1");
        return ESP_ERR_NO_MEM;
    }
    
    lv_color_t *buf2 = (lv_color_t*)heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    if (buf2 == NULL) {
        free(buf1);
        ESP_LOGE(TAG, "Failed to allocate LVGL buffer 2");
        return ESP_ERR_NO_MEM;
    }
 
   ESP_LOGI(TAG, "Register display driver to LVGL");
   lv_display_set_user_data(disp, panel_handle);
   lv_display_set_flush_cb(disp, lvgl_flush_cb);
   lv_display_add_event_cb(disp, lvgl_rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);
   // Buffer size is expressed in bytes in v9 (not pixel count); partial render
   // mode matches the old double-buffer / partial-flush behavior.
   lv_display_set_buffers(disp, buf1, buf2,
                           EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
 
   // Carries over your lv_conf.h v8 setting LV_COLOR_16_SWAP = 1: in v9 the
   // swap is a runtime display color format instead of a compile-time flag.
   lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
 
   // Apply the default (unrotated) panel orientation. Equivalent to what
   // drv_update_cb used to do once automatically at registration time.
   apply_panel_rotation(panel_handle, LV_DISPLAY_ROTATION_0);
 
   ESP_LOGI(TAG, "Install LVGL tick timer");
   // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
   const esp_timer_create_args_t lvgl_tick_timer_args = {
       .callback = &increase_lvgl_tick,
       .name = "lvgl_tick"
   };
   esp_timer_handle_t lvgl_tick_timer = NULL;
   ret = esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to create LVGL tick timer");
       return ret;
   }
   
   ret = esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);
   if (ret != ESP_OK) {
       ESP_LOGE(TAG, "Failed to start LVGL tick timer");
       return ret;
   }
 
 #if EXAMPLE_USE_TOUCH
   ESP_LOGI(TAG, "Register touch input device to LVGL");
   lv_indev_t *indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
   lv_indev_set_read_cb(indev, lvgl_touch_cb);
   lv_indev_set_display(indev, disp);
 #endif
 
   lvgl_mux = xSemaphoreCreateMutex();
   if (lvgl_mux == NULL) {
       ESP_LOGE(TAG, "Failed to create LVGL mutex");
       return ESP_ERR_NO_MEM;
   }
 
   ESP_LOGI(TAG, "Create LVGL task");
   BaseType_t task_created = xTaskCreate(lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
   if (task_created != pdPASS) {
       ESP_LOGE(TAG, "Failed to create LVGL task");
       return ESP_FAIL;
   }
 
   ESP_LOGI(TAG, "Display initialization completed successfully");
   return ESP_OK;
 }
 
 void display_set_rotation(lv_display_rotation_t rotation)
 {
   if (s_disp == NULL || s_panel_handle == NULL) {
       ESP_LOGW(TAG, "display_init must be called first");
       return;
   }
   lv_display_set_rotation(s_disp, rotation);
   apply_panel_rotation(s_panel_handle, rotation);
 }
 
 bool display_lvgl_lock(int timeout_ms)
 {
   if (lvgl_mux == NULL) {
       ESP_LOGW(TAG, "display_init must be called first");
       return false;
   }
 
   const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
   return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
 }
 
 void display_lvgl_unlock(void)
 {
   if (lvgl_mux == NULL) {
       ESP_LOGW(TAG, "display_init must be called first");
       return;
   }
   xSemaphoreGive(lvgl_mux);
 }