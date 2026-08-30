#include <Arduino.h>
#include "esp_log.h"
#include "display_manager.h"
#include "ui.h" // UI header genere par SquareLine Studio

#include "playback/jukebox.h"
#include "sd/music_library.h"
#include "audio/audio_player.h"
#include "ui/ui_now_playing.h"

static const char *TAG = "main";

// Instances globales (referencees via "extern" dans ui_events.cpp)
Jukebox playback;
AudioPlayer player;

void setup()
{
  Serial.begin(115200);
  ESP_LOGI(TAG, "Application starting...");

  // Graine aleatoire pour le mode shuffle (sinon meme sequence a chaque boot)
  randomSeed(esp_random());

  Serial.printf("[Debug] Boot : Heap interne libre=%u | PSRAM libre=%u\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

  // IMPORTANT : on initialise l'I2S (audio) AVANT l'ecran/LVGL.
  // L'I2S a besoin de RAM INTERNE (pas la PSRAM) pour ses descripteurs DMA.
  // Si l'ecran/LVGL s'initialisent en premier et consomment toute la RAM
  // interne disponible pour leurs buffers, l'I2S echoue a l'allocation
  // (symptome observe : "i2s_alloc_dma_desc... allocate DMA buffer failed").
  // En reservant sa (petite) part de RAM interne en tout premier, l'I2S
  // s'initialise correctement avant que LVGL ne prenne le reste.
  player.begin();
  Serial.printf("[Debug] Apres player.begin() : Heap interne libre=%u | PSRAM libre=%u\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

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

  Serial.printf("[Debug] Apres ecran+UI : Heap interne libre=%u | PSRAM libre=%u\n",
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

  // Carte SD + bibliotheque musicale
  if (!MusicLibrary::begin())
  {
    ESP_LOGE(TAG, "Echec montage carte SD");
    return;
  }
  ESP_LOGI(TAG, "Carte SD montee");

  const char *musicFolder = "/";
  size_t added = MusicLibrary::loadFolder(playback, musicFolder, true);
  ESP_LOGI(TAG, "%d piste(s) chargee(s) depuis %s", (int)added, musicFolder);

  if (playback.empty())
  {
    ESP_LOGW(TAG, "Aucune piste trouvee sur la carte SD");
    return;
  }

  // Le Jukebox est pret, l'I2S deja initialise plus haut : on peut lancer la lecture
  player.attachJukebox(playback);
  player.playCurrent();

  ESP_LOGI(TAG, "Setup complete");
}

void loop()
{
  // La tache LVGL gere l'affichage separement (voir display_manager).
  // Ici on ne fait que faire avancer le decodeur audio.
  player.loop();

  static uint32_t lastUiUpdate = 0;
  if (millis() - lastUiUpdate > 250)
  {
    lastUiUpdate = millis();
    updateNowPlayingUI();
  }

  vTaskDelay(pdMS_TO_TICKS(1));
}