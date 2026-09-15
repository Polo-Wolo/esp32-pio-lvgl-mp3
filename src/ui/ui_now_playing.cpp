#include "ui_now_playing.h"
#include "ui.h"
#include "screens.h"
#include "display_manager.h"

#include "audio/audio_player.h"

// Defini dans main.cpp
extern AudioPlayer player;

// Cache pour n'ecrire les labels/le slider que quand quelque chose change
// (evite des redraws LVGL inutiles a chaque appel).
static String lastTitle, lastArtist, lastAlbum;
static uint32_t lastDuration = 0;
static uint32_t lastCurrent = (uint32_t)-1;

// Accedes uniquement sous le verrou LVGL (callbacks et rafraichissement).
static bool seekPending = false;
static uint32_t seekTarget = 0, seekStarted = 0, seekRequest = 0, seekTrack = 0;

static void showCurrentTime(uint32_t seconds)
{
    if (seconds == lastCurrent)
        return;
    lv_label_set_text_fmt(objects.track_current_time, "%02d:%02d",
                         (int)(seconds / 60), (int)(seconds % 60));
    lastCurrent = seconds;
}

void initNowPlayingUI()
{
    auto slider = objects.track_seek_slider;
    lv_obj_add_event_cb(slider, handleSeekSliderEvent, LV_EVENT_RELEASED, nullptr);
    lv_obj_add_event_cb(slider, handleSeekSliderEvent, LV_EVENT_PRESS_LOST, nullptr);
    // Un glissement du slider ne doit pas aussi declencher un swipe de piste.
    lv_obj_remove_flag(slider, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_remove_flag(slider, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

void handleSeekSliderEvent(lv_event_t *e)
{
    auto slider = (lv_obj_t *)lv_event_get_target(e);
    const auto code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESS_LOST)
    {
        seekPending = false;
        return;
    }
    const uint32_t target = (uint32_t)lv_slider_get_value(slider);
    showCurrentTime(target); // Retour visuel immediat, sans acceder au decodeur.
    if (code == LV_EVENT_VALUE_CHANGED && lv_slider_is_dragged(slider))
        return; // Un seul seek au relachement du doigt.
    if (code != LV_EVENT_VALUE_CHANGED && code != LV_EVENT_RELEASED)
        return;

    const auto state = player.state();
    if (++seekRequest == 0) ++seekRequest;
    seekPending = player.seekTo(target, seekRequest);
    seekTarget = target;
    seekStarted = millis();
    seekTrack = state.trackGeneration;
    if (!seekPending)
        Serial.println("[Seek] Commande refusee : file pleine ou player indisponible.");
}

void updateNowPlayingUI()
{
    if (!display_lvgl_lock(10))
        return;
    // Lire apres le verrou : un callback a pu envoyer un seek pendant l'attente.
    const auto state = player.state();
    if (!state.hasTrack)
    {
        display_lvgl_unlock();
        return;
    }

    uint32_t cur = state.currentTime;
    uint32_t dur = state.duration;

    bool userIsDragging = lv_slider_is_dragged(objects.track_seek_slider) ||
                          lv_obj_has_state(objects.track_seek_slider, LV_STATE_PRESSED);
    if (seekPending)
    {
        const bool acknowledged = state.seekRequest == seekRequest;
        const int64_t distance = (int64_t)cur - seekTarget;
        const bool arrived = distance >= -2 && distance <= 2;
        // L'acceptation par la lib precede le repositionnement effectif.
        // Attendre aussi le temps reel ; ne jamais figer l'UI indefiniment.
        if (state.trackGeneration != seekTrack ||
            (acknowledged && (!state.seekAccepted || arrived)) ||
            millis() - seekStarted >= 2000)
            seekPending = false;
    }
    if (userIsDragging)
        cur = (uint32_t)lv_slider_get_value(objects.track_seek_slider);
    else if (seekPending)
        cur = seekTarget;

    if (!userIsDragging && dur != lastDuration)
    {
        lv_slider_set_range(objects.track_seek_slider, 0, (int32_t)dur);
        lastDuration = dur;
    }
    if (!userIsDragging)
    {
        // Pas d'animation qui ramene progressivement vers un ancien etat.
        if (lv_slider_get_value(objects.track_seek_slider) != (int32_t)cur)
            lv_slider_set_value(objects.track_seek_slider, (int32_t)cur, LV_ANIM_OFF);
    }

    // --- Temps ecoule / duree totale ---
    showCurrentTime(cur);
    lv_label_set_text_fmt(objects.track_duration, "%02d:%02d", (int)(dur / 60), (int)(dur % 60));

    // --- Textes ---
    if (state.title != lastTitle)
    {
        lv_label_set_text(objects.track_text, state.title.c_str());
        lastTitle = state.title;
    }
    if (state.artist != lastArtist)
    {
        lv_label_set_text(objects.artist_text, state.artist.c_str());
        lastArtist = state.artist;
    }
    if (state.album != lastAlbum)
    {
        lv_label_set_text(objects.album_text, state.album.c_str());
        lastAlbum = state.album;
    }

    display_lvgl_unlock();
}
