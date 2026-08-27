/**
 * @file main.cpp
 * @brief Minimal LVGL v9 test: bright background + one touch button.
 *        Used to isolate whether the black-screen issue is in the panel/flush
 *        path or in the LVGL object tree.
 */

#include <Arduino.h>
#include "display_manager.h"
#include "lvgl.h"

static const char *TAG = "main";

static void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if (code != LV_EVENT_CLICKED) {
    return;
  }

  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t *label = lv_obj_get_child(btn, 0);

  static uint32_t click_count = 0;
  click_count++;

  Serial.printf("[%s] Button clicked, count=%lu\n", TAG, (unsigned long)click_count);
  lv_label_set_text_fmt(label, "Clicked: %lu", (unsigned long)click_count);
}

void setup()
{
  Serial.begin(115200);
  delay(300);
  Serial.println("Booting...");

  esp_err_t ret = display_init();
  if (ret != ESP_OK) {
    Serial.printf("display_init() FAILED: %d\n", ret);
    return;
  }
  Serial.println("display_init() OK");

  // display_init() already spins up its own FreeRTOS task (lvgl_port_task)
  // which calls lv_timer_handler() in a loop, so any LVGL call made from
  // setup()/loop() must be wrapped in display_lvgl_lock()/unlock() to avoid
  // touching LVGL's non-thread-safe state from two tasks at once.
  if (!display_lvgl_lock(-1)) {
    Serial.println("Could not lock LVGL mutex");
    return;
  }

  lv_obj_t *scr = lv_screen_active();

  // Deliberately loud, non-default color: if this never shows up on the
  // panel, the problem is downstream of LVGL (flush / SPI / backlight),
  // not a missing widget or a theme issue.
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x003a57), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *btn = lv_button_create(scr);
  lv_obj_set_size(btn, 160, 60);
  lv_obj_center(btn);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, "Touch me");
  lv_obj_center(label);

  display_lvgl_unlock();

  Serial.println("UI created");
}

void loop()
{
  // Nothing to do here: lvgl_port_task (started inside display_init())
  // already handles lv_timer_handler() on its own schedule.
  vTaskDelay(pdMS_TO_TICKS(1000));
}