/**
 * @file lv_conf.h
 * Configuration file for v9.5.0
 * Adapted from a customized LVGL v8.3.11 lv_conf.h
 */

/* clang-format off */
#if 1 /* Set this to "1" to enable content — enabled to match the previous v8 config */

#ifndef LV_CONF_H
#define LV_CONF_H

#if  0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*====================
   COLOR SETTINGS
 *====================*/

/** Color depth: 1 (I1), 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/

#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#define LV_STDINT_INCLUDE       <stdint.h>
#define LV_STDDEF_INCLUDE       <stddef.h>
#define LV_STDBOOL_INCLUDE      <stdbool.h>
#define LV_INTTYPES_INCLUDE     <inttypes.h>
#define LV_LIMITS_INCLUDE       <limits.h>
#define LV_STDARG_INCLUDE       <stdarg.h>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /* Was 48 KB in your v8 config (default was 32 KB) — kept the same increase */
    #define LV_MEM_SIZE (48 * 1024U)          /**< [bytes] */

    #define LV_MEM_POOL_EXPAND_SIZE 0

    #define LV_MEM_ADR 0     /**< 0: unused*/
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC
    #endif
#endif

/*====================
   HAL SETTINGS
 *====================*/

/* v8 had LV_DISP_DEF_REFR_PERIOD = 30 and LV_INDEV_DEF_READ_PERIOD = 30.
 * v9 merged both into a single period, kept at 30. */
#define LV_DEF_REFR_PERIOD  30      /**< [ms] */

#define LV_DPI_DEF 130              /**< [px/inch] */

/*=================
 * OPERATING SYSTEM
 *=================*/
#define LV_USE_OS   LV_OS_NONE

#if LV_USE_OS == LV_OS_CUSTOM
    #define LV_OS_CUSTOM_INCLUDE <stdint.h>
#endif
#if LV_USE_OS == LV_OS_FREERTOS
    #define LV_USE_FREERTOS_TASK_NOTIFY 1
#endif

/*========================
 * RENDERING CONFIGURATION
 *========================*/

#define LV_DRAW_BUF_STRIDE_ALIGN                1
#define LV_DRAW_BUF_ALIGN                       4
#define LV_DRAW_TRANSFORM_USE_MATRIX            0

#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /**< [bytes], same as your v8 LV_LAYER_SIMPLE_BUF_SIZE */
/* NOTE: v9 no longer has a separate "fallback" buffer size
 * (your v8 LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE has no v9 equivalent). */
#define LV_DRAW_LAYER_MAX_MEMORY 0  /**< No limit by default [bytes]*/

#define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes]*/
#define LV_DRAW_THREAD_PRIO LV_THREAD_PRIO_HIGH

#define LV_USE_DRAW_SW 1
#if LV_USE_DRAW_SW == 1
    #define LV_DRAW_SW_SUPPORT_RGB565       1
    /* Kept enabled: this is the closest v9 equivalent to your v8 LV_COLOR_16_SWAP = 1.
     * IMPORTANT: in v9 the actual byte-swap is no longer a compile-time lv_conf.h flag.
     * You now select it at runtime on the display, e.g.:
     *   lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);
     * Make sure to set that in your display driver init code. */
    #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED       1
    #define LV_DRAW_SW_SUPPORT_RGB565A8     1
    #define LV_DRAW_SW_SUPPORT_RGB888       1
    #define LV_DRAW_SW_SUPPORT_XRGB8888     1
    #define LV_DRAW_SW_SUPPORT_ARGB8888     1
    #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1
    #define LV_DRAW_SW_SUPPORT_L8           1
    #define LV_DRAW_SW_SUPPORT_AL88         1
    #define LV_DRAW_SW_SUPPORT_A8           1
    #define LV_DRAW_SW_SUPPORT_I1           1

    #define LV_DRAW_SW_I1_LUM_THRESHOLD 127

    #define LV_DRAW_SW_DRAW_UNIT_CNT    1
    #define LV_USE_DRAW_ARM2D_SYNC      0
    #define LV_USE_NATIVE_HELIUM_ASM    0

    /* Same as your v8 LV_DRAW_COMPLEX = 1 */
    #define LV_DRAW_SW_COMPLEX          1

    #if LV_DRAW_SW_COMPLEX == 1
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0      /* same as v8 LV_SHADOW_CACHE_SIZE */
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4       /* same as v8 LV_CIRCLE_CACHE_SIZE */
    #endif

    #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
    #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
        #define  LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
    #endif

    /* v8's LV_DITHER_GRADIENT / LV_DITHER_ERROR_DIFFUSION have no direct v9 knob;
     * complex (angled/radial/conical) gradients are now gated by this flag instead. */
    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0
#endif

#define LV_USE_NEMA_GFX 0
#if LV_USE_NEMA_GFX
    #define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE
    #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
    #if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
        #define LV_NEMA_STM32_HAL_INCLUDE <stm32u5xx_hal.h>
        #define LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
    #endif
    #define LV_USE_NEMA_VG 0
    #if LV_USE_NEMA_VG
        #define LV_NEMA_GFX_MAX_RESX 800
        #define LV_NEMA_GFX_MAX_RESY 600
    #endif
#endif

#define LV_USE_PXP 0
#if LV_USE_PXP
    #define LV_USE_DRAW_PXP 1
    #define LV_USE_ROTATE_PXP 0
    #if LV_USE_DRAW_PXP && LV_USE_OS
        #define LV_USE_PXP_DRAW_THREAD 1
    #endif
    #define LV_USE_PXP_ASSERT 0
#endif

#define LV_USE_G2D 0
#if LV_USE_G2D
    #define LV_USE_DRAW_G2D 1
    #define LV_USE_ROTATE_G2D 0
    #define LV_G2D_HASH_TABLE_SIZE 50
    #if LV_USE_DRAW_G2D && LV_USE_OS
        #define LV_USE_G2D_DRAW_THREAD 1
    #endif
    #define LV_USE_G2D_ASSERT 0
#endif

#define LV_USE_DRAW_DAVE2D 0
#define LV_USE_DRAW_SDL 0

#define LV_USE_DRAW_VG_LITE 0
#if LV_USE_DRAW_VG_LITE
    #define LV_VG_LITE_USE_GPU_INIT 0
    #define LV_VG_LITE_USE_ASSERT 0
    #define LV_VG_LITE_FLUSH_MAX_COUNT 8
    #define LV_VG_LITE_USE_BOX_SHADOW 1
    #define LV_VG_LITE_GRAD_CACHE_CNT 32
    #define LV_VG_LITE_STROKE_CACHE_CNT 32
    #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256
    #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0
    #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0
    #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0
    #define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000
    #define LV_USE_VG_LITE_DRIVER  0
    #if LV_USE_VG_LITE_DRIVER
        #define LV_VG_LITE_HAL_GPU_SERIES gc255
        #define LV_VG_LITE_HAL_GPU_REVISION 0x40
        #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000
    #endif
    #define LV_USE_VG_LITE_THORVG   0
    #if LV_USE_VG_LITE_THORVG
        #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0
        #define LV_VG_LITE_THORVG_YUV_SUPPORT 0
        #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0
        #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1
        #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64
        #define LV_VG_LITE_THORVG_THREAD_RENDER 0
    #endif
#endif

#define LV_USE_DRAW_DMA2D 0
#if LV_USE_DRAW_DMA2D
    #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"
    #define LV_USE_DRAW_DMA2D_INTERRUPT 0
#endif

#define LV_USE_DRAW_OPENGLES 0
#if LV_USE_DRAW_OPENGLES
    #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64
#endif

#define LV_USE_PPA  0
#if LV_USE_PPA
    #define LV_USE_PPA_IMG      0
    #define LV_PPA_BURST_LENGTH    128
#endif

#define LV_USE_DRAW_EVE 0
#if LV_USE_DRAW_EVE
    #define LV_DRAW_EVE_EVE_GENERATION 4
    #define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048
#endif

#define LV_USE_DRAW_NANOVG 0
#if LV_USE_DRAW_NANOVG
    #define LV_NANOVG_BACKEND   LV_NANOVG_BACKEND_GLES2
    #define LV_NANOVG_IMAGE_CACHE_CNT 128
    #define LV_NANOVG_LETTER_CACHE_CNT 512
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Logging
 *-----------*/
#define LV_USE_LOG 0
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF 0
    #define LV_LOG_USE_TIMESTAMP 1
    #define LV_LOG_USE_FILE_LINE 1
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1
    #define LV_LOG_TRACE_CACHE      1
#endif

/*-------------
 * Asserts
 *-----------*/
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);     /**< Halt by default, same as your v8 config */

/*-------------
 * Debug
 *-----------*/
#define LV_USE_REFR_DEBUG 0
#define LV_USE_LAYER_DEBUG 0
#define LV_USE_PARALLEL_DRAW_DEBUG 0

/*-------------
 * Others
 *-----------*/
#define LV_ENABLE_GLOBAL_CUSTOM 0
#if LV_ENABLE_GLOBAL_CUSTOM
    #define LV_GLOBAL_CUSTOM_INCLUDE <stdint.h>
#endif

/* Your v8 config had LV_IMG_CACHE_DEF_SIZE = 1 (cache 1 image).
 * v9 measures the image cache by BYTES, not image count, so it isn't a 1:1 port.
 * Set to a small non-zero size to keep image caching enabled — tune to your needs. */
#define LV_CACHE_DEF_SIZE       (32 * 1024)

#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0

#define LV_GRADIENT_MAX_STOPS   2      /* same as your v8 LV_GRADIENT_MAX_STOPS */

#define LV_COLOR_MIX_ROUND_OFS  0      /* same as your v8 value */

#define LV_OBJ_STYLE_CACHE      0
#define LV_USE_OBJ_ID           0
#define LV_USE_OBJ_NAME         0
#define LV_OBJ_ID_AUTO_ASSIGN   LV_USE_OBJ_ID
#define LV_USE_OBJ_ID_BUILTIN   1
#define LV_USE_OBJ_PROPERTY 0
#define LV_USE_OBJ_PROPERTY_NAME 1
#define LV_USE_GESTURE_RECOGNITION 0

/*=====================
 *  COMPILER SETTINGS
 *====================*/
#define LV_BIG_ENDIAN_SYSTEM 0
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_FAST_MEM
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#define LV_ATTRIBUTE_EXTERN_DATA

#define LV_USE_FLOAT            0
#define LV_USE_MATRIX           0

#ifndef LV_USE_PRIVATE_API
    #define LV_USE_PRIVATE_API  0
#endif

/*==================
 *   FONT USAGE
 *===================*/
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
/* Enabled to match your v8 config (v9 default is 0) */
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
/* Enabled to match your v8 config (v9 default is 0) */
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

#define LV_FONT_MONTSERRAT_28_COMPRESSED    0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    0
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK   0
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK   0

#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

#define LV_FONT_CUSTOM_DECLARE

#define LV_FONT_DEFAULT &lv_font_montserrat_14

#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 *  TEXT SETTINGS
 *=================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_)]}"
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

#define LV_USE_BIDI 0
#if LV_USE_BIDI
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

#define LV_USE_ARABIC_PERSIAN_CHARS 0
#define LV_TXT_COLOR_CMD "#"

/*==================
 * WIDGETS
 *================*/
#define LV_WIDGETS_HAS_DEFAULT_VALUE  1

#define LV_USE_ANIMIMG    1
#define LV_USE_ARC        1
#define LV_USE_ARCLABEL   1
#define LV_USE_BAR        1
/* Renamed from LV_USE_BTN in v8 */
#define LV_USE_BUTTON        1
/* Renamed from LV_USE_BTNMATRIX in v8 */
#define LV_USE_BUTTONMATRIX  1

#define LV_USE_CALENDAR   1
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif
    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW 1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
    #define LV_USE_CALENDAR_CHINESE 0
#endif

#define LV_USE_CANVAS     1
#define LV_USE_CHART      1
#define LV_USE_CHECKBOX   1
#define LV_USE_DROPDOWN   1

/* Renamed from LV_USE_IMG in v8 */
#define LV_USE_IMAGE      1
/* Renamed from LV_USE_IMGBTN in v8 */
#define LV_USE_IMAGEBUTTON     1

#define LV_USE_KEYBOARD   1

#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1
    #define LV_LABEL_LONG_TXT_HINT 1
    #define LV_LABEL_WAIT_CHAR_COUNT 3
#endif

#define LV_USE_LED        1
#define LV_USE_LINE       1
#define LV_USE_LIST       1
#define LV_USE_LOTTIE     0
#define LV_USE_MENU       1
#define LV_USE_MSGBOX     1
/* Renamed from LV_USE_ROLLER in v8. Note: v9 dropped the LV_ROLLER_INF_PAGES sub-setting. */
#define LV_USE_ROLLER     1

/* NOTE: v8's LV_USE_METER (meter widget) was removed in v9 and replaced by the
 * more general LV_USE_SCALE widget — enabled below as the closest equivalent. */
#define LV_USE_SCALE      1

#define LV_USE_SLIDER     1
#define LV_USE_SPAN       1
#if LV_USE_SPAN
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif
#define LV_USE_SPINBOX    1
#define LV_USE_SPINNER    1
#define LV_USE_SWITCH     1
#define LV_USE_TABLE      1
#define LV_USE_TABVIEW    1

#define LV_USE_TEXTAREA   1
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500
#endif

#define LV_USE_TILEVIEW   1
#define LV_USE_WIN        1
#define LV_USE_3DTEXTURE  0

/* NOTE: v8's LV_USE_COLORWHEEL is no longer part of the core v9 widget set shown here;
 * if you relied on it, check the current LVGL widget list for its v9 status/location. */

/*==================
 * THEMES
 *==================*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    #define LV_THEME_DEFAULT_DARK 0
    #define LV_THEME_DEFAULT_GROW 1
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

/* Renamed from LV_USE_THEME_BASIC in v8 */
#define LV_USE_THEME_SIMPLE 1
#define LV_USE_THEME_MONO 1

/*==================
 * LAYOUTS
 *==================*/
#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
 * 3RD PARTY LIBRARIES
 *====================*/
#define LV_FS_DEFAULT_DRIVER_LETTER '\0'

#define LV_USE_FS_STDIO 0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'
    #define LV_FS_STDIO_PATH ""
    #define LV_FS_STDIO_CACHE_SIZE 0
#endif

#define LV_USE_FS_POSIX 0
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER '\0'
    #define LV_FS_POSIX_PATH ""
    #define LV_FS_POSIX_CACHE_SIZE 0
#endif

#define LV_USE_FS_WIN32 0
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER '\0'
    #define LV_FS_WIN32_PATH ""
    #define LV_FS_WIN32_CACHE_SIZE 0
#endif

/* Same as your v8 LV_USE_FS_FATFS (was 0, driver letter 'S' unused since disabled) */
#define LV_USE_FS_FATFS 0
#if LV_USE_FS_FATFS
    #define LV_FS_FATFS_LETTER '\0'
    #define LV_FS_FATFS_PATH ""
    #define LV_FS_FATFS_CACHE_SIZE 0
#endif

#define LV_USE_FS_MEMFS 0
#if LV_USE_FS_MEMFS
    #define LV_FS_MEMFS_LETTER '\0'
#endif

#define LV_USE_FS_LITTLEFS 0
#if LV_USE_FS_LITTLEFS
    #define LV_FS_LITTLEFS_LETTER '\0'
    #define LV_FS_LITTLEFS_PATH ""
#endif

#define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER '\0'
    #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""
#endif

#define LV_USE_FS_ARDUINO_SD 0
#if LV_USE_FS_ARDUINO_SD
    #define LV_FS_ARDUINO_SD_LETTER '\0'
    #define LV_FS_ARDUINO_SD_PATH ""
#endif

#define LV_USE_FS_UEFI 0
#if LV_USE_FS_UEFI
    #define LV_FS_UEFI_LETTER '\0'
#endif

#define LV_USE_FS_FROGFS 0
#if LV_USE_FS_FROGFS
    #define LV_FS_FROGFS_LETTER '\0'
#endif

#define LV_USE_LODEPNG 0
#define LV_USE_LIBPNG 0
#define LV_USE_BMP 0

/* Renamed/replaced: v8's LV_USE_SJPG (split-JPG decoder) was enabled (1) in your config.
 * v9's equivalent built-in JPG decoder is LV_USE_TJPGD — enabled to match. */
#define LV_USE_TJPGD 1

#define LV_USE_LIBJPEG_TURBO 0
#define LV_USE_LIBWEBP 0

/* Enabled to match your v8 LV_USE_GIF = 1 */
#define LV_USE_GIF 1
#if LV_USE_GIF
    #define LV_GIF_CACHE_DECODE_DATA 0
#endif

#define LV_USE_GSTREAMER 0
#define LV_BIN_DECODER_RAM_LOAD 0
#define LV_USE_RLE 0
#define LV_USE_QRCODE 0
#define LV_USE_BARCODE 0

#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
    #define LV_FREETYPE_USE_LVGL_PORT 0
    #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
#endif

#define LV_USE_TINY_TTF 0
#if LV_USE_TINY_TTF
    #define LV_TINY_TTF_FILE_SUPPORT 0
    #define LV_TINY_TTF_CACHE_GLYPH_CNT 128
    #define LV_TINY_TTF_CACHE_KERNING_CNT 256
#endif

#define LV_USE_RLOTTIE 0
#define LV_USE_GLTF  0

#define LV_USE_VECTOR_GRAPHIC  0
#define LV_USE_THORVG_INTERNAL 0
#define LV_USE_THORVG_EXTERNAL 0
#define LV_USE_NANOVG 0
#define LV_USE_LZ4_INTERNAL  0
#define LV_USE_LZ4_EXTERNAL  0

#define LV_USE_SVG 0
#define LV_USE_SVG_ANIMATION 0
#define LV_USE_SVG_DEBUG 0

#define LV_USE_FFMPEG 0
#if LV_USE_FFMPEG
    #define LV_FFMPEG_DUMP_FORMAT 0
    #define LV_FFMPEG_PLAYER_USE_LV_FS 0
#endif

/*==================
 * OTHERS
 *==================*/
#define LV_USE_SNAPSHOT 0

#define LV_USE_SYSMON   0
#if LV_USE_SYSMON
    #define LV_SYSMON_GET_IDLE lv_os_get_idle_percent
    #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
    #if LV_SYSMON_PROC_IDLE_AVAILABLE
        #define LV_SYSMON_GET_PROC_IDLE lv_os_get_proc_idle_percent
    #endif

    /* Same as your v8 LV_USE_PERF_MONITOR = 0 */
    #define LV_USE_PERF_MONITOR 0
    #if LV_USE_PERF_MONITOR
        #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
        #define LV_USE_PERF_MONITOR_LOG_MODE 0
    #endif

    /* Same as your v8 LV_USE_MEM_MONITOR = 0 */
    #define LV_USE_MEM_MONITOR 0
    #if LV_USE_MEM_MONITOR
        #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
    #endif
#endif

#define LV_USE_PROFILER 0
#if LV_USE_PROFILER
    #define LV_USE_PROFILER_BUILTIN 1
    #if LV_USE_PROFILER_BUILTIN
        #define LV_PROFILER_BUILTIN_BUF_SIZE (16 * 1024)
        #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1
        #define LV_USE_PROFILER_BUILTIN_POSIX 0
    #endif
    #define LV_PROFILER_INCLUDE "lvgl/src/misc/lv_profiler_builtin.h"
    #define LV_PROFILER_BEGIN    LV_PROFILER_BUILTIN_BEGIN
    #define LV_PROFILER_END      LV_PROFILER_BUILTIN_END
    #define LV_PROFILER_BEGIN_TAG LV_PROFILER_BUILTIN_BEGIN_TAG
    #define LV_PROFILER_END_TAG   LV_PROFILER_BUILTIN_END_TAG
    #define LV_PROFILER_LAYOUT 1
    #define LV_PROFILER_REFR 1
    #define LV_PROFILER_DRAW 1
    #define LV_PROFILER_INDEV 1
    #define LV_PROFILER_DECODER 1
    #define LV_PROFILER_FONT 1
    #define LV_PROFILER_FS 1
    #define LV_PROFILER_STYLE 0
    #define LV_PROFILER_TIMER 1
    #define LV_PROFILER_CACHE 1
    #define LV_PROFILER_EVENT 1
#endif

#define LV_USE_MONKEY 0
#define LV_USE_GRIDNAV 0
#define LV_USE_FRAGMENT 0
#define LV_USE_IMGFONT 0

/* Renamed/replaced: v8's LV_USE_MSG (pub/sub messaging) is superseded in v9
 * by the more general Observer pattern. Kept enabled (v9 default). */
#define LV_USE_OBSERVER 1

#define LV_USE_IME_PINYIN 0
#if LV_USE_IME_PINYIN
    #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
    #define LV_IME_PINYIN_CAND_TEXT_NUM 6
    #define LV_IME_PINYIN_USE_K9_MODE      1
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif
#endif

#define LV_USE_FILE_EXPLORER                     0
#if LV_USE_FILE_EXPLORER
    #define LV_FILE_EXPLORER_PATH_MAX_LEN        (128)
    #define LV_FILE_EXPLORER_QUICK_ACCESS        1
#endif

#define LV_USE_FONT_MANAGER                     0
#if LV_USE_FONT_MANAGER
    #define LV_FONT_MANAGER_NAME_MAX_LEN            32
#endif

#define LV_USE_TEST 0
#if LV_USE_TEST
    #define LV_USE_TEST_SCREENSHOT_COMPARE 0
    #if LV_USE_TEST_SCREENSHOT_COMPARE
        #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1
    #endif
#endif

#define LV_USE_TRANSLATION 0
#define LV_USE_COLOR_FILTER     0

/*==================
 * DEVICES
 *==================*/
#define LV_USE_SDL              0
#if LV_USE_SDL
    #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
    #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT
    #define LV_SDL_BUF_COUNT        1
    #define LV_SDL_ACCELERATED      1
    #define LV_SDL_FULLSCREEN       0
    #define LV_SDL_DIRECT_EXIT      1
    #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER
#endif

#define LV_USE_X11              0
#if LV_USE_X11
    #define LV_X11_DIRECT_EXIT         1
    #define LV_X11_DOUBLE_BUFFER       1
    #define LV_X11_RENDER_MODE_PARTIAL 1
    #define LV_X11_RENDER_MODE_DIRECT  0
    #define LV_X11_RENDER_MODE_FULL    0
#endif

#define LV_USE_WAYLAND          0
#if LV_USE_WAYLAND
    #define LV_WAYLAND_DIRECT_EXIT          1
#endif

#define LV_USE_LINUX_FBDEV      0
#if LV_USE_LINUX_FBDEV
    #define LV_LINUX_FBDEV_BSD           0
    #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
    #define LV_LINUX_FBDEV_BUFFER_COUNT  0
    #define LV_LINUX_FBDEV_BUFFER_SIZE   60
    #define LV_LINUX_FBDEV_MMAP          1
#endif

#define LV_USE_NUTTX    0
#if LV_USE_NUTTX
    #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0
    #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP    0
    #define LV_USE_NUTTX_LIBUV    0
    #define LV_USE_NUTTX_CUSTOM_INIT    0
    #define LV_USE_NUTTX_LCD      0
    #if LV_USE_NUTTX_LCD
        #define LV_NUTTX_LCD_BUFFER_COUNT    0
        #define LV_NUTTX_LCD_BUFFER_SIZE     60
    #endif
    #define LV_USE_NUTTX_TOUCHSCREEN    0
    #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE    0
    #define LV_USE_NUTTX_MOUSE    0
    #define LV_USE_NUTTX_MOUSE_MOVE_STEP    1
    #define LV_USE_NUTTX_TRACE_FILE 0
    #if LV_USE_NUTTX_TRACE_FILE
        #define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"
    #endif
#endif

#define LV_USE_LINUX_DRM        0
#if LV_USE_LINUX_DRM
    #define LV_USE_LINUX_DRM_GBM_BUFFERS 0
#endif

#define LV_USE_TFT_ESPI         0
#define LV_USE_LOVYAN_GFX         0
#if LV_USE_LOVYAN_GFX
    #define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"
#endif

#define LV_USE_EVDEV    0
#define LV_USE_LIBINPUT    0
#if LV_USE_LIBINPUT
    #define LV_LIBINPUT_BSD    0
    #define LV_LIBINPUT_XKB             0
    #if LV_LIBINPUT_XKB
        #define LV_LIBINPUT_XKB_KEY_MAP { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
    #endif
#endif

#define LV_USE_ST7735        0
#define LV_USE_ST7789        0
#define LV_USE_ST7796        0
#define LV_USE_ILI9341       0
#define LV_USE_FT81X         0
#define LV_USE_NV3007        0

#if (LV_USE_ST7735 | LV_USE_ST7789 | LV_USE_ST7796 | LV_USE_ILI9341 | LV_USE_NV3007)
    #define LV_USE_GENERIC_MIPI 1
#else
    #define LV_USE_GENERIC_MIPI 0
#endif

#define LV_USE_RENESAS_GLCDC    0

#define LV_USE_ST_LTDC    0
#if LV_USE_ST_LTDC
    #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
#endif

#define LV_USE_NXP_ELCDIF   0
#define LV_USE_WINDOWS    0

#define LV_USE_UEFI 0
#if LV_USE_UEFI
    #define LV_USE_UEFI_INCLUDE "myefi.h"
    #define LV_UEFI_USE_MEMORY_SERVICES 0
#endif

#define LV_USE_OPENGLES   0
#if LV_USE_OPENGLES
    #define LV_USE_OPENGLES_DEBUG        1
#endif

#define LV_USE_GLFW   0

#define LV_USE_QNX              0
#if LV_USE_QNX
    #define LV_QNX_BUF_COUNT        1
#endif

#define LV_USE_EXT_DATA   0

/*=====================
* BUILD OPTIONS
*======================*/

/* Enabled to match your v8 LV_BUILD_EXAMPLES = 1 */
#define LV_BUILD_EXAMPLES 1

#define LV_BUILD_DEMOS 1

/*===================
 * DEMO USAGE
 ====================*/
#if LV_BUILD_DEMOS
    /* Enabled to match your v8 LV_USE_DEMO_WIDGETS = 1 */
    #define LV_USE_DEMO_WIDGETS 0

    #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #define LV_USE_DEMO_BENCHMARK 0
    #if LV_USE_DEMO_BENCHMARK
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0
    #endif

    #define LV_USE_DEMO_RENDER 0
    #define LV_USE_DEMO_STRESS 0

    #define LV_USE_DEMO_MUSIC 0
    #if LV_USE_DEMO_MUSIC
        #define LV_DEMO_MUSIC_SQUARE    0
        #define LV_DEMO_MUSIC_LANDSCAPE 0
        #define LV_DEMO_MUSIC_ROUND     0
        #define LV_DEMO_MUSIC_LARGE     0
        #define LV_DEMO_MUSIC_AUTO_PLAY 0
    #endif

    #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    #define LV_USE_DEMO_GLTF            0
    #define LV_USE_DEMO_FLEX_LAYOUT     0
    #define LV_USE_DEMO_MULTILANG       0

    #define LV_USE_DEMO_EBIKE           0
    #if LV_USE_DEMO_EBIKE
        #define LV_DEMO_EBIKE_PORTRAIT  0
    #endif

    #define LV_USE_DEMO_HIGH_RES        0
    #define LV_USE_DEMO_SMARTWATCH      0
#endif /* LV_BUILD_DEMOS */

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/