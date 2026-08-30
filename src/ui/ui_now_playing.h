#pragma once

// Met a jour l'affichage "lecture en cours" (slider de progression,
// titre/artiste/album) a partir de l'etat reel du Jukebox/AudioPlayer.
// A appeler periodiquement depuis loop() (pas depuis un callback LVGL,
// donc protege son acces a LVGL via display_lvgl_lock()/unlock()).
void updateNowPlayingUI();