#include "ui_now_playing.h"
#include "ui.h"
#include "screens.h"
#include "display_manager.h"
#include "playback/jukebox.h"
#include "audio/audio_player.h"

// Definis dans main.cpp
extern Jukebox playback;
extern AudioPlayer player;

// Cache pour n'ecrire les labels/le slider que quand quelque chose change
// (evite des redraws LVGL inutiles a chaque appel).
static String lastTitle, lastArtist, lastAlbum;
static uint32_t lastDuration = 0;
static uint32_t lastCurrent = (uint32_t)-1;

void updateNowPlayingUI()
{
    const Music *track = playback.current();
    if (!track)
        return;

    uint32_t cur = player.currentTime();
    uint32_t dur = player.duration();

    // Rien de nouveau a afficher : on sort avant meme de tenter le lock LVGL
    bool textChanged = (track->title != lastTitle) || (track->artist != lastArtist) || (track->album != lastAlbum);
    bool timeChanged = (cur != lastCurrent) || (dur != lastDuration);
    if (!textChanged && !timeChanged)
        return;

    if (!display_lvgl_lock(10))
        return; // n'attend pas indefiniment si le mutex est pris

    // --- Slider de progression : SEEK_SLIDER a remplacer par le vrai nom de votre objet ---
    if (dur != lastDuration)
    {
        lv_slider_set_range(objects.track_seek_slider, 0, (int32_t)dur);
        lastDuration = dur;
    }
    lv_slider_set_value(objects.track_seek_slider, (int32_t)cur, LV_ANIM_ON);
    lastCurrent = cur;

    // --- Textes ---
    if (track->title != lastTitle)
    {
        lv_label_set_text(objects.track_text, track->title.c_str());
        lastTitle = track->title;
    }
    if (track->artist != lastArtist)
    {
        lv_label_set_text(objects.artist_text, track->artist.c_str());
        lastArtist = track->artist;
    }
    if (track->album != lastAlbum)
    {
        lv_label_set_text(objects.album_text, track->album.c_str());
        lastAlbum = track->album;
    }

    display_lvgl_unlock();
}