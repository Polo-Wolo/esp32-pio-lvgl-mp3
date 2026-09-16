#include "cover_thumbnail.h"
#include <Arduino.h>
#include <esp_heap_caps.h>
#include <libs/tjpgd/tjpgd.h>

namespace {
constexpr unsigned Side = 160;
struct DecodeContext {
    const uint8_t *data;
    size_t size, position;
    uint16_t *pixels;
    unsigned width, height, left, top;
    bool (*cancelled)(void *);
    void *context;
    uint32_t lastYield;
};

size_t readJpeg(JDEC *decoder, uint8_t *buffer, size_t count)
{
    auto &ctx = *static_cast<DecodeContext *>(decoder->device);
    if (ctx.cancelled(ctx.context)) return 0;
    if (count > ctx.size - ctx.position) count = ctx.size - ctx.position;
    if (buffer) memcpy(buffer, ctx.data + ctx.position, count);
    ctx.position += count;
    return count;
}

int writeBlock(JDEC *decoder, void *bitmap, JRECT *rect)
{
    auto &ctx = *static_cast<DecodeContext *>(decoder->device);
    if (ctx.cancelled(ctx.context)) return 0;
    const auto *bgr = static_cast<const uint8_t *>(bitmap);
    if (millis() - ctx.lastYield >= 4) {
        vTaskDelay(1);
        ctx.lastYield = millis();
    }
    const unsigned stride = rect->right - rect->left + 1;
    // Chaque pixel destination choisit un pixel source. Les bornes arrondies
    // vers le haut evitent trous et chevauchements entre blocs JPEG.
    const unsigned x0 = (unsigned(rect->left) * ctx.width + decoder->width - 1) / decoder->width;
    const unsigned x1 = ((unsigned(rect->right) + 1) * ctx.width + decoder->width - 1) / decoder->width;
    const unsigned y0 = (unsigned(rect->top) * ctx.height + decoder->height - 1) / decoder->height;
    const unsigned y1 = ((unsigned(rect->bottom) + 1) * ctx.height + decoder->height - 1) / decoder->height;
    for (unsigned y = y0; y < y1; ++y) {
        const unsigned sy = y * decoder->height / ctx.height - rect->top;
        for (unsigned x = x0; x < x1; ++x) {
            const unsigned sx = x * decoder->width / ctx.width - rect->left;
            const uint8_t *p = bgr + (sy * stride + sx) * 3;
            // La version TJPGD de LVGL produit B, G, R.
            ctx.pixels[(y + ctx.top) * Side + x + ctx.left] =
                uint16_t((p[2] >> 3) << 11 | (p[1] >> 2) << 5 | (p[0] >> 3));
        }
    }
    return 1;
}
}

uint16_t *decodeCoverThumbnail(const uint8_t *data, size_t size, lv_image_dsc_t &image,
                              bool (*cancelled)(void *), void *context)
{
    static_assert(JD_FORMAT == 0, "Le redimensionnement attend la sortie BGR888 de TJPGD");
    auto *work = static_cast<uint8_t *>(malloc(4096));
    if (!work) {
        Serial.println("[Cover] Memoire insuffisante pour le decodeur.");
        return nullptr;
    }
    DecodeContext ctx{data, size, 0, nullptr, 0, 0, 0, 0, cancelled, context, millis()};
    JDEC decoder{};
    JRESULT result = jd_prepare(&decoder, readJpeg, work, 4096, &ctx);
    // Borner aussi le cout CPU : un petit JPEG compresse peut etre immense.
    if (result == JDR_OK && decoder.width && decoder.height &&
        uint32_t(decoder.width) * decoder.height <= 4096UL * 4096) {
        const unsigned longest = decoder.width > decoder.height ? decoder.width : decoder.height;
        ctx.width = unsigned(decoder.width) * Side / longest;
        ctx.height = unsigned(decoder.height) * Side / longest;
        if (!ctx.width) ctx.width = 1;
        if (!ctx.height) ctx.height = 1;
        ctx.left = (Side - ctx.width) / 2;
        ctx.top = (Side - ctx.height) / 2;
        ctx.pixels = static_cast<uint16_t *>(heap_caps_malloc(Side * Side * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
        if (ctx.pixels) {
            // Meme fond #3f3f3f que le widget, pour les pochettes non carrees.
            for (unsigned i = 0; i < Side * Side; ++i) ctx.pixels[i] = 0x39e7;
            result = jd_decomp(&decoder, writeBlock, 0);
        }
        else result = JDR_MEM1;
    }
    else if (result == JDR_OK) result = JDR_FMT1;
    free(work);
    if (result != JDR_OK) {
        if (result != JDR_INTR && !cancelled(context))
            Serial.printf("[Cover] Echec decodage JPEG, code TJPGD=%d\n", int(result));
        free(ctx.pixels);
        return nullptr;
    }
    image = {};
    image.header.magic = LV_IMAGE_HEADER_MAGIC;
    image.header.cf = LV_COLOR_FORMAT_RGB565;
    image.header.w = Side;
    image.header.h = Side;
    image.header.stride = Side * 2;
    image.data_size = Side * Side * 2;
    image.data = reinterpret_cast<const uint8_t *>(ctx.pixels);
    Serial.printf("[Cover] JPEG %u x %u -> %u x %u dans cadre 160 x 160\n",
                  unsigned(decoder.width), unsigned(decoder.height), ctx.width, ctx.height);
    return ctx.pixels;
}
