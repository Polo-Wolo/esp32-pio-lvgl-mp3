#include <Arduino.h>
#include "ui_events.h"
#include "ui.h"      // genere par SquareLine Studio : declare ui_player, ui_browser...
#include "actions.h" // declare action_play_pause_btn, action_next_btn...
#include "screens.h" // declare "objects" (objects.play_pause, objects.ttttest...)
#include "images.h"  // declare img_play, img_pause...
#include "playback/jukebox.h"
#include "audio/audio_player.h"

#define DEBUG_UI_EVENTS 0

// Definis dans main.cpp
extern Jukebox playback;
extern AudioPlayer player;

// Ecran actuellement affiche, pour savoir dans quel sens naviguer au swipe
enum class UIScreen
{
    UIScreenPlayer,
    UIScreenBrowser
};

enum class UiChild
{
    UiChildNone = -1,
    UiChildTrackImage,
    UiChildTrackInfos,
    UiChildCount
};

UiChild currentChild = UiChild::UiChildNone;
static UIScreen currentScreen = UIScreen::UIScreenPlayer;

// ==================================================
// BOUTONS DE LECTURE
// ==================================================

void action_play_pause_btn(lv_event_t *e)
{
    player.pauseResume();

    bool running = player.isRunning();
    lv_image_set_src(objects.icon_play_pause, running ? &img_pause : &img_play); // v9 : lv_img_set_src -> lv_image_set_src

    if (DEBUG_UI_EVENTS)
        Serial.println(running ? "[Lecture]" : "[Pause]");
}

void action_next_btn(lv_event_t *e)
{
    player.next();
    const Music *track = playback.current();
    if (track)
    {
        if (DEBUG_UI_EVENTS)
            Serial.printf("[Next] %s\n", track->title.c_str());
    }
}

void action_prev_btn(lv_event_t *e)
{
    player.previous();
    const Music *track = playback.current();
    if (track)
    {
        if (DEBUG_UI_EVENTS)
            Serial.printf("[Prev] %s\n", track->title.c_str());
    }
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
    {
        if (DEBUG_UI_EVENTS)
            Serial.printf("[Like] %s\n", track->title.c_str());
    }
}

// ==================================================
// SEEK : glisser le slider deplace la lecture
// ==================================================

void action_seek_slider_changed(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider); // en secondes (range = duree de la piste)

    player.seekTo((uint32_t)value);

    // NB: LV_EVENT_VALUE_CHANGED se declenche en continu pendant le glissement,
    // donc seekTo() est appele plusieurs fois par seconde tant que le doigt bouge.
    // Si ca provoque des saccades audio, remplacez LV_EVENT_VALUE_CHANGED par
    // LV_EVENT_RELEASED dans EEZ Studio pour ne chercher qu'au relachement.
}

const char *childToString(UiChild child)
{
    switch (child)
    {
    case UiChild::UiChildNone:
        return "None";
    case UiChild::UiChildTrackImage:
        return "TrackImage";
    case UiChild::UiChildTrackInfos:
        return "TrackInfos";
    default:
        return "Unknown";
    }
}

void action_child_pressed(lv_event_t *e)
{
    int data = (int)lv_event_get_user_data(e);
    if (data >= 0 && data < (int)UiChild::UiChildCount)
        currentChild = (UiChild)data;
    else
        currentChild = UiChild::UiChildNone;
    if (DEBUG_UI_EVENTS)
        Serial.printf("[CHILD] Current Child : %s\n", childToString(currentChild));
}

void action_child_clicked(lv_event_t *e)
{
    currentChild = UiChild::UiChildNone;
    if (DEBUG_UI_EVENTS)
        Serial.printf("[CHILD] RESET : %s\n", childToString(currentChild));
}

// ==================================================
// GESTES PAGE : swipe gauche/droite = changer d'ecran
// ==================================================
void handle_player_gesture(lv_dir_t dir)
{
    switch (dir)
    {
    case LV_DIR_LEFT:
        action_next_btn(nullptr);
        break;

    case LV_DIR_RIGHT:
        action_prev_btn(nullptr);
        break;

    case LV_DIR_TOP:
        if (player.getVolume() < 21)
            player.setVolume(player.getVolume() + 1);
        break;

    case LV_DIR_BOTTOM:
        if (player.getVolume() > 0)
            player.setVolume(player.getVolume() - 1);
    default:
        break;
    }
}

void handle_browser_gesture(lv_dir_t dir)
{
    switch (dir)
    {
    case LV_DIR_LEFT:
        if (currentScreen == UIScreen::UIScreenBrowser)
        {
            loadScreen(SCREEN_ID_UI_PLAYER);
            currentScreen = UIScreen::UIScreenPlayer;
        }
        break;
    case LV_DIR_RIGHT:
        if (currentScreen == UIScreen::UIScreenPlayer)
        {
            loadScreen(SCREEN_ID_UI_BROWSER);
            currentScreen = UIScreen::UIScreenBrowser;
        }
        break;
    default:
        break;
    }
}

void action_gesture(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active()); // v9 : lv_indev_get_act -> lv_indev_active

    switch (currentChild)
    {
    case UiChild::UiChildTrackImage:
        handle_browser_gesture(dir);
        break;
    case UiChild::UiChildTrackInfos:
        handle_player_gesture(dir);
        break;
    }
}

