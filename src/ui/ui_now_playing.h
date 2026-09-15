#pragma once
#include <lvgl.h>

// Apres ui_init(), sous le verrou LVGL. Ajoute les evenements de fin de geste
// sans modifier les fichiers generes par l'editeur UI.
void initNowPlayingUI();
void handleSeekSliderEvent(lv_event_t *e);

// Met a jour l'affichage "lecture en cours" (slider de progression,
// titre/artiste/album) a partir de l'etat reel du Jukebox/AudioPlayer.
// A appeler periodiquement depuis loop() (pas depuis un callback LVGL,
// donc protege son acces a LVGL via display_lvgl_lock()/unlock()).
void updateNowPlayingUI();
