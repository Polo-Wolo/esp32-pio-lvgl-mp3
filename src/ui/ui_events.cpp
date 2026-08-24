#include <Arduino.h>
#include "ui_events.h"
#include "ui.h" // genere par SquareLine Studio : declare ui_player, ui_browser...
#include "screens.h" // genere par SquareLine Studio : declare loadScreen()
#include "images.h"  // genere par SquareLine Studio : declare img_skip_back, img_skip_next, img_play, img_pause...
#include "playback/jukebox.h"
#include "audio/audio_player.h"



// Definis dans main.cpp
extern Jukebox playback;
extern AudioPlayer player;

// Ecran actuellement affiche, pour savoir dans quel sens naviguer au swipe
enum class UIScreen
{
    NOW_PLAYING,
    BROWSER
};
static UIScreen currentScreen = UIScreen::NOW_PLAYING;

// ==================================================
// BOUTONS DE LECTURE
// ==================================================

void action_play_pause_btn(lv_event_t *e)
{
    player.pauseResume();

    bool running = player.isRunning();
    lv_img_set_src(objects.icon_play_pause, running ? &img_pause : &img_play);

    Serial.println(running ? "[Lecture]" : "[Pause]");
}

void action_next_btn(lv_event_t *e)
{
    player.next();
    const Music *track = playback.current();
    if (track)
        Serial.printf("[Next] %s\n", track->title.c_str());
}

void action_prev_btn(lv_event_t *e)
{
    player.previous();
    const Music *track = playback.current();
    if (track)
        Serial.printf("[Prev] %s\n", track->title.c_str());
}

void action_shuffle_btn(lv_event_t *e)
{
    playback.toggleShuffle();
    // TODO : mettre a jour l'icone du bouton selon playback.isShuffleEnabled()
}

void action_repeat_btn(lv_event_t *e)
{
    playback.cycleRepeatMode();
    // TODO : mettre a jour l'icone du bouton selon playback.repeatMode() (OFF/ALL/ONE)
}

void action_like_btn(lv_event_t *e)
{
    // TODO : brancher sur une future gestion de favoris/playlists (.m3u)
    const Music *track = playback.current();
    if (track)
        Serial.printf("[Like] %s\n", track->title.c_str());
}

// ==================================================
// GESTES : swipe gauche/droite = changer d'ecran
// ==================================================

void action_gesture(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    switch (dir)
    {
    case LV_DIR_LEFT:
        if (currentScreen == UIScreen::NOW_PLAYING)
        {
            Serial.println("[Geste] LEFT -> Browser");
            // lv_scr_load_anim(ui_browser, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, false);
            loadScreen(SCREEN_ID_UI_PLAYER);
            currentScreen = UIScreen::BROWSER;
        }
        break;

    case LV_DIR_RIGHT:
        if (currentScreen == UIScreen::BROWSER)
        {
            Serial.println("[Geste] RIGHT -> Now Playing");
            // lv_scr_load_anim(ui_player, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, false);
            loadScreen(SCREEN_ID_UI_BROWSER);
            currentScreen = UIScreen::NOW_PLAYING;
        }
        break;

    case LV_DIR_TOP:
        Serial.println("[Geste] TOP -> volume +");
        if (player.getVolume() < 21)
            player.setVolume(player.getVolume() + 1);
        break;

    case LV_DIR_BOTTOM:
        Serial.println("[Geste] BOTTOM -> volume -");
        if (player.getVolume() > 0)
            player.setVolume(player.getVolume() - 1);
        break;

    default:
        break;
    }
}