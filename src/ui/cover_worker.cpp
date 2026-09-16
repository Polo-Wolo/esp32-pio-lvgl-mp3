#include "cover_worker.h"
#include "cover_thumbnail.h"
#include <SD_MMC.h>
#include <esp_heap_caps.h>
#include <atomic>
#include <freertos/semphr.h>

namespace {
struct Job {
    String path;
    std::vector<uint32_t> segments;
    uint32_t generation;
    uint32_t ticket;
};
SemaphoreHandle_t mutex = nullptr;
TaskHandle_t task = nullptr;
std::shared_ptr<Job> pending;
std::shared_ptr<const CoverThumbnail> ready;
uint32_t readyGeneration = 0;
std::atomic<uint32_t> ticket{0};

bool cancelled(void *context)
{
    return ticket.load() != static_cast<Job *>(context)->ticket;
}

void run(void *)
{
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        xSemaphoreTake(mutex, portMAX_DELAY);
        auto job = std::move(pending);
        xSemaphoreGive(mutex);
        if (!job || cancelled(job.get())) continue;
        const uint32_t started = millis();
        File file = SD_MMC.open(job->path, FILE_READ);
        if (!file) {
            Serial.println("[Cover] Impossible d'ouvrir le fichier audio.");
            continue;
        }
        for (size_t s = 0; s + 1 < job->segments.size() && !cancelled(job.get()); s += 2) {
            const uint32_t offset = job->segments[s], size = job->segments[s + 1];
            if (size < 4 || size > 512 * 1024 || offset > file.size() ||
                size > file.size() - offset || !file.seek(offset)) continue;
            // Pas de gros JPEG dans le tas interne reserve notamment a l'audio.
            std::unique_ptr<uint8_t, decltype(&free)> bytes(
                static_cast<uint8_t *>(heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)), free);
            if (!bytes) {
                Serial.println("[Cover] PSRAM insuffisante pour le JPEG.");
                break;
            }
            size_t read = 0;
            while (read < size && !cancelled(job.get())) {
                const size_t chunk = (size - read > 4096) ? 4096 : size - read;
                const size_t n = file.read(bytes.get() + read, chunk);
                if (!n) break;
                read += n;
                vTaskDelay(1); // Laisser servir le flux audio entre les lectures SD.
            }
            if (cancelled(job.get())) break;
            if (read != size) continue;
            size_t start = 0;
            while (start + 1 < size && !(bytes.get()[start] == 0xff && bytes.get()[start + 1] == 0xd8)) ++start;
            if (start + 1 >= size) continue;
            auto thumbnail = std::make_shared<CoverThumbnail>();
            thumbnail->pixels = decodeCoverThumbnail(bytes.get() + start, size - start,
                                                     thumbnail->image, cancelled, job.get());
            // Liberer le JPEG compresse des que la vignette est terminee.
            bytes.reset();
            if (!thumbnail->pixels) continue;
            xSemaphoreTake(mutex, portMAX_DELAY);
            if (!cancelled(job.get())) {
                ready = thumbnail;
                readyGeneration = job->generation;
            }
            xSemaphoreGive(mutex);
            Serial.printf("[Cover] Traitement fond : %lu ms, vignette 50 Kio\n", (unsigned long)(millis() - started));
            break;
        }
    }
}
}

bool CoverWorker::begin()
{
    if (task) return true;
    mutex = xSemaphoreCreateMutex();
    if (!mutex) return false;
    if (xTaskCreate(run, "Cover", 6144, nullptr, 1, &task) != pdPASS) {
        vSemaphoreDelete(mutex);
        mutex = nullptr;
        return false;
    }
    return true;
}

void CoverWorker::reset(uint32_t generation)
{
    if (!mutex) return;
    xSemaphoreTake(mutex, portMAX_DELAY);
    ++ticket;
    pending.reset();
    ready.reset();
    readyGeneration = generation;
    xSemaphoreGive(mutex);
}

void CoverWorker::request(const String &path, const std::vector<uint32_t> &segments, uint32_t generation)
{
    if (!mutex) return;
    auto job = std::make_shared<Job>();
    job->path = path;
    job->segments = segments;
    job->generation = generation;
    xSemaphoreTake(mutex, portMAX_DELAY);
    job->ticket = ++ticket;
    pending = job; // Une seule demande en attente : la plus recente remplace l'ancienne.
    xSemaphoreGive(mutex);
    xTaskNotifyGive(task);
}

std::shared_ptr<const CoverThumbnail> CoverWorker::result(uint32_t generation)
{
    if (!mutex) return {};
    xSemaphoreTake(mutex, portMAX_DELAY);
    auto value = readyGeneration == generation ? ready : nullptr;
    xSemaphoreGive(mutex);
    return value;
}
