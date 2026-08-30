#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_UI_PLAYER = 1,
    SCREEN_ID_UI_BROWSER = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *ui_player;
    lv_obj_t *ui_browser;
    lv_obj_t *previous;
    lv_obj_t *icon_previous;
    lv_obj_t *next;
    lv_obj_t *icon_next;
    lv_obj_t *play_pause;
    lv_obj_t *icon_play_pause;
    lv_obj_t *music_mode;
    lv_obj_t *obj0;
    lv_obj_t *track_iimage;
    lv_obj_t *obj1;
    lv_obj_t *track_text;
    lv_obj_t *artist_text;
    lv_obj_t *album_text;
    lv_obj_t *track_current_time;
    lv_obj_t *track_duration;
    lv_obj_t *track_seek_slider;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_span_t *span_0;
} screen_ui_browser_state_t;

extern screen_ui_browser_state_t screen_ui_browser_state;

void create_screen_ui_player();
void tick_screen_ui_player();

void create_screen_ui_browser();
void tick_screen_ui_browser();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/