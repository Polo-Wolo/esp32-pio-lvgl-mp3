#include <Arduino.h>
#include "esp_log.h"
#include "display_manager.h"
#include "ui.h" // UI header genere par SquareLine Studio

#include "playback/jukebox.h"
#include "sd/music_library.h"
#include "audio/audio_player.h"

static const char *TAG = "main";

// Instances globales (referencees via "extern" dans ui_events.cpp)
Jukebox     playback;
AudioPlayer player;

void setup()
{
    Serial.begin(115200);
    ESP_LOGI(TAG, "Application starting...");

    // Graine aleatoire pour le mode shuffle (sinon meme sequence a chaque boot)
    randomSeed(esp_random());

    // Initialisation ecran + LVGL
    esp_err_t ret = display_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Display initialization failed with error: %d", ret);
        return;
    }

    // Initialisation UI (SquareLine Studio)
    if (display_lvgl_lock(-1))
    {
        ESP_LOGI(TAG, "Initializing UI");
        ui_init();
        display_lvgl_unlock();
        ESP_LOGI(TAG, "UI initialization complete");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to acquire LVGL mutex for UI initialization");
        return;
    }

    // Carte SD + bibliotheque musicale
    if (!MusicLibrary::begin())
    {
        ESP_LOGE(TAG, "Echec montage carte SD");
        return;
    }
    ESP_LOGI(TAG, "Carte SD montee");

    size_t added = MusicLibrary::loadFolder(playback, "/", true);
    ESP_LOGI(TAG, "%d piste(s) chargee(s) depuis /", (int)added);

    if (playback.empty())
    {
        ESP_LOGW(TAG, "Aucune piste trouvee sur la carte SD");
        return;
    }

    // Audio I2S relie au Jukebox
    player.begin();
    player.attachJukebox(playback);
    player.playCurrent();

    ESP_LOGI(TAG, "Setup complete");
}

void loop()
{
    // La tache LVGL gere l'affichage separement (voir display_manager).
    // Ici on ne fait que faire avancer le decodeur audio.
    player.loop();
    vTaskDelay(pdMS_TO_TICKS(1));
}