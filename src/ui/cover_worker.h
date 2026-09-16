#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <memory>
#include <vector>

struct CoverThumbnail {
    CoverThumbnail() = default;
    CoverThumbnail(const CoverThumbnail &) = delete;
    CoverThumbnail &operator=(const CoverThumbnail &) = delete;
    lv_image_dsc_t image{};
    uint16_t *pixels = nullptr;
    ~CoverThumbnail() { free(pixels); }
};

// Proprietaire des demandes : tache audio. La tache de fond ne touche pas LVGL.
namespace CoverWorker {
bool begin();
void reset(uint32_t generation);
void request(const String &path, const std::vector<uint32_t> &segments, uint32_t generation);
std::shared_ptr<const CoverThumbnail> result(uint32_t generation);
}
