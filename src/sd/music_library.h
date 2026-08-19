#pragma once
#include <Arduino.h>
#include "playback/jukebox.h"

// Chargement de la bibliotheque musicale depuis la carte SD (SDMMC)
namespace MusicLibrary
{
    // Monte la carte SD (mode SDMMC 1-bit). A appeler une fois dans setup().
    bool begin();

    // Parcourt "folderPath" et ajoute chaque .mp3 trouve au Jukebox
    // (titre = nom de fichier sans extension, artiste/album vides pour l'instant,
    // seront mis a jour a la lecture via les tags ID3).
    // Si recursive = true, parcourt aussi les sous-dossiers.
    // Retourne le nombre de pistes ajoutees.
    size_t loadFolder(Jukebox &pm, const String &folderPath, bool recursive = false);
}
