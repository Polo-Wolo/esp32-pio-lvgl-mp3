#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

screen_ui_browser_state_t screen_ui_browser_state;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_ui_player() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.ui_player = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 536, 240);
    lv_obj_add_event_cb(obj, action_gesture, LV_EVENT_GESTURE, (void *)0);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 178, 178);
            lv_obj_set_size(obj, 102, 62);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 102, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 62, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_LEFT|LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_transform_height(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f0f0f), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, 19, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_skip_back);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 434, 178);
            lv_obj_set_size(obj, 102, 62);
            lv_obj_add_event_cb(obj, action_next_btn, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 102, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 62, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_transform_height(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f0f0f), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, 19, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_skip_forward);
                }
            }
        }
        {
            // PlayPause
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.play_pause = obj;
            lv_obj_set_pos(obj, 280, 178);
            lv_obj_set_size(obj, 154, 62);
            lv_obj_add_event_cb(obj, action_play_pause_btn, LV_EVENT_CLICKED, (void *)0);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 62, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_LEFT|LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_transform_height(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f0f0f), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // icon_play_pause
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.icon_play_pause = obj;
                    lv_obj_set_pos(obj, 45, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_play);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 0, 178);
            lv_obj_set_size(obj, 83, 62);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 89, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 62, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_transform_height(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f0f0f), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, 13, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_repeat);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 89, 178);
            lv_obj_set_size(obj, 89, 62);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 62, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 89, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_LEFT|LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_transform_width(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_transform_height(obj, 0, LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x0f0f0f), LV_PART_MAIN | LV_STATE_PRESSED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    lv_obj_set_pos(obj, 13, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_heart);
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 178, 178);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x4a79f2), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_height(obj, 178, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_width(obj, 178, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM|LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_img_src(obj, &img_placeholder, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj5 = obj;
            lv_obj_set_pos(obj, 178, 0);
            lv_obj_set_size(obj, 358, 166);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_event_cb(obj, action_gesture, LV_EVENT_GESTURE, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM|LV_BORDER_SIDE_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj6 = obj;
                    lv_obj_set_pos(obj, 14, 14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "On Melancholy Hill");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 14, 56);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_letter_space(obj, -1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Gorillaz");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj8 = obj;
                    lv_obj_set_pos(obj, 14, 95);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_opa(obj, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_letter_space(obj, -1, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "Plastic Beach");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, 9, 142);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "0:04");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, 323, 142);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "3:13");
                }
            }
        }
        {
            lv_obj_t *obj = lv_slider_create(parent_obj);
            objects.obj11 = obj;
            lv_obj_set_pos(obj, 178, 166);
            lv_obj_set_size(obj, 358, 12);
            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
            lv_obj_set_style_height(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x181818), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_BOTTOM|LV_BORDER_SIDE_TOP|LV_BORDER_SIDE_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 2, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 4, LV_PART_KNOB | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_ui_player();
}

void tick_screen_ui_player() {
}

void create_screen_ui_browser() {
    screen_ui_browser_state_t *state = &screen_ui_browser_state;
    (void)state;
    lv_obj_t *obj = lv_obj_create(0);
    objects.ui_browser = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 536, 240);
    lv_obj_add_event_cb(obj, action_gesture, LV_EVENT_GESTURE, (void *)0);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_list_create(parent_obj);
            lv_obj_set_pos(obj, 41, 30);
            lv_obj_set_size(obj, 382, 162);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_spangroup_create(parent_obj);
                    lv_obj_set_pos(obj, 59, 80);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    {
                        state->span_0 = lv_spangroup_new_span(obj);
                        lv_span_set_text_static(state->span_0, "Span");
                    }
                    lv_spangroup_refr_mode(obj);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 69, 71);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text_static(obj, "Text");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 140, 136);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text_static(obj, "Text");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 203, 209);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text_static(obj, "Text");
                }
            }
        }
    }
    
    tick_screen_ui_browser();
}

void tick_screen_ui_browser() {
    screen_ui_browser_state_t *state = &screen_ui_browser_state;
    (void)state;
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_ui_player,
    tick_screen_ui_browser,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 2) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_ui_player();
    create_screen_ui_browser();
}