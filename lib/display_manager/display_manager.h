/**
 * @file display_manager.h
 * @brief Header file for the display and touch manager functionality
 */

 #ifndef DISPLAY_MANAGER_H
 #define DISPLAY_MANAGER_H
 
 #include <Arduino.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_lcd_panel_io.h"
 #include "esp_lcd_panel_ops.h"
 #include "lvgl.h"
 
 // Configuration defines
 #define LCD_HOST    SPI2_HOST
 #define TOUCH_HOST  I2C_NUM_0
 
 #define LCD_BIT_PER_PIXEL       (16)
 
 #define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
 #define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
 #define EXAMPLE_PIN_NUM_LCD_CS            (GPIO_NUM_6)
 #define EXAMPLE_PIN_NUM_LCD_PCLK          (GPIO_NUM_47) 
 #define EXAMPLE_PIN_NUM_LCD_DATA0         (GPIO_NUM_18)
 #define EXAMPLE_PIN_NUM_LCD_DATA1         (GPIO_NUM_7)
 #define EXAMPLE_PIN_NUM_LCD_DATA2         (GPIO_NUM_48)
 #define EXAMPLE_PIN_NUM_LCD_DATA3         (GPIO_NUM_5)
 #define EXAMPLE_PIN_NUM_LCD_RST           (GPIO_NUM_17)
 #define EXAMPLE_PIN_NUM_BK_LIGHT          (-1)
 
 // The pixel number in horizontal and vertical
 #define EXAMPLE_LCD_H_RES              536    
 #define EXAMPLE_LCD_V_RES              240
 
 #define EXAMPLE_USE_TOUCH              1 // Touch enabled
 
 #define EXAMPLE_LVGL_BUF_HEIGHT        (EXAMPLE_LCD_V_RES/4)
 #define EXAMPLE_LVGL_TICK_PERIOD_MS    2
 #define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
 #define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1
 // v9 runs the whole draw pipeline (default theme, widget rendering, rounding)
 // inline in this task since LV_USE_OS == LV_OS_NONE (no separate draw thread).
 // 4 KB (the old v8 value) is not enough and causes a stack-canary panic on
 // first render. Start at 12 KB; you can right-size later by checking
 // uxTaskGetStackHighWaterMark() on the "LVGL" task once it's stable.
 #define EXAMPLE_LVGL_TASK_STACK_SIZE   (12 * 1024)
 #define EXAMPLE_LVGL_TASK_PRIORITY     2
 
 /**
  * @brief Initialize the display, touch, and LVGL
  * 
  * Sets up the SPI bus, LCD panel, touch controller, and initializes LVGL
  * 
  * @return ESP_OK if successful, otherwise an error code
  */
 esp_err_t display_init();
 
 /**
  * @brief Set the display rotation
  *
  * Updates both LVGL's internal rotation state and the physical panel's
  * swap/mirror settings to match. In LVGL v8 this happened automatically
  * whenever lv_disp_set_rotation() was called (via disp_drv.drv_update_cb);
  * v9 dropped that automatic hook, so call this function instead of
  * lv_display_set_rotation() directly if you need to rotate at runtime.
  *
  * @param rotation One of LV_DISPLAY_ROTATION_0/90/180/270
  */
 void display_set_rotation(lv_display_rotation_t rotation);
 
 /**
  * @brief Lock the LVGL mutex
  * 
  * @param timeout_ms Timeout in milliseconds, -1 for infinite wait
  * @return true if the mutex was successfully acquired, false otherwise
  */
 bool display_lvgl_lock(int timeout_ms);
 
 /**
  * @brief Unlock the LVGL mutex
  */
 void display_lvgl_unlock();
 
 #endif // DISPLAY_MANAGER_H