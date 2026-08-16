#include <Arduino.h>
#include "ui_events.h"
#include "Audio.h"

extern Audio audio;

void action_play_pause_btn(lv_event_t *e)
{
    audio.pauseResume();
    Serial.println(audio.isRunning() ? "[Lecture]" : "[Pause]");
}

void action_next_btn(lv_event_t *e)
{
    Serial.println("NEXT");
}

void action_like_btn(lv_event_t *e)
{
    Serial.println("like");
}

void action_gesture(lv_event_t *e)
{
    char ret[6] = "";
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    switch (dir)
    {
    case LV_DIR_LEFT:
        snprintf(ret, sizeof(ret), "LEFT");
        break;
    case LV_DIR_RIGHT:
        snprintf(ret, sizeof(ret), "RIGHT");
        break;
    case LV_DIR_TOP:
        snprintf(ret, sizeof(ret), "TOP");
        break;
    case LV_DIR_BOTTOM:
        snprintf(ret, sizeof(ret), "BOTTOM");
        break;
    }
    Serial.println(ret);
}