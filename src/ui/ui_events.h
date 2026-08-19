#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

void action_play_pause_btn(lv_event_t * e);
void action_next_btn(lv_event_t * e);
void action_like_btn(lv_event_t * e);
void action_gesture(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif