#pragma once

#include <lvgl.h>
#include <stddef.h>
#include <stdint.h>

// Le buffer RGB565 retourne appartient a l'appelant (free apres retrait du cache).
uint16_t *decodeCoverThumbnail(const uint8_t *data, size_t size, lv_image_dsc_t &image,
                              bool (*cancelled)(void *), void *context);
