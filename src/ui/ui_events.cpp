#include <Arduino.h>
#include "ui_events.h"
#include "ui.h"      // genere par SquareLine Studio : declare ui_player, ui_browser...
#include "actions.h" // declare action_play_pause_btn, action_next_btn...
#include "screens.h" // declare "objects" (objects.play_pause, objects.ttttest...)
#include "images.h"  // declare img_play, img_pause...

#include "audio/audio_player.h"
#include "ui_now_playing.h"

#define DEBUG_UI_EVENTS 0

// Defini dans main.cpp
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

// Pose a true par action_gesture() des qu'un swipe est detecte sur la sequence
// tactile en cours. action_child_clicked() le verifie pour ignorer le clic
// qui suit un swipe (LVGL declenche CLICKED meme apres un geste sur le meme
// objet). Remis a zero au debut de chaque nouvelle pression.
static bool s_gestureHandled = false;

// ==================================================
// BOUTONS DE LECTURE
// ==================================================

// Une commande refusee est signalee ; aucune attente dans la tache LVGL.
static void reportCommand(bool accepted)
{
    if (!accepted)
        Serial.println("[AudioPlayer] Commande refusee : file pleine ou player indisponible.");
}

void action_play_pause_btn(lv_event_t *e)
{
    reportCommand(player.pauseResume());
}

void action_next_btn(lv_event_t *e)
{
    reportCommand(player.next());
}

void action_prev_btn(lv_event_t *e)
{
    reportCommand(player.previousOrRestart());
}

void action_shuffle_btn(lv_event_t *e)
{
    reportCommand(player.toggleShuffle());
}

void action_repeat_btn(lv_event_t *e)
{
    reportCommand(player.cycleRepeatMode());
}

void action_like_btn(lv_event_t *e)
{
    // TODO : favoris/playlists. Lire uniquement la copie publiee.
    if (DEBUG_UI_EVENTS)
    {
        auto state = player.state();
        if (state.hasTrack)
            Serial.printf("[Like] %s\n", state.title.c_str());
    }
}
// ==================================================
// SEEK : glisser le slider deplace la lecture
// ==================================================

void action_seek_slider_changed(lv_event_t *e)
{
    handleSeekSliderEvent(e);
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
    s_gestureHandled = false; // nouvelle pression : on oublie l'eventuel geste precedent

    int data = (int)lv_event_get_user_data(e);
    switch (data)
    {
    case 0:
        currentChild = UiChild::UiChildTrackImage;
        break;
    case 1:
        currentChild = UiChild::UiChildTrackInfos;
        break;
    default:
        currentChild = UiChild::UiChildNone;
    }
    if (data >= 0 && data < (int)UiChild::UiChildCount)
        currentChild = (UiChild)data;

    if (DEBUG_UI_EVENTS)
        Serial.printf("[CHILD] Current Child : %s\n", childToString(currentChild));
}

void action_child_clicked(lv_event_t *e)
{
    // --- Comparaison diagnostique, ne change aucun comportement ---
    // On veut voir si lv_indev_get_gesture_dir() aurait pu remplacer
    // s_gestureHandled. Si les deux logs concordent toujours (true <-> != NONE),
    // on pourra simplifier plus tard. S'ils divergent, on garde le drapeau maison.
    lv_dir_t indevDir = lv_indev_get_gesture_dir(lv_indev_active());
    Serial.printf("[DEBUG-GESTURE] s_gestureHandled=%s | lv_indev_get_gesture_dir=%d (NONE=%d)\n",
                  s_gestureHandled ? "true" : "false", (int)indevDir, (int)LV_DIR_NONE);

    if (s_gestureHandled)
    {
        // Ce "clic" est la fin d'un swipe deja traite par action_gesture() :
        // on l'ignore, sinon un swipe declenche AUSSI play/pause.
        s_gestureHandled = false;
        currentChild = UiChild::UiChildNone;
        if (DEBUG_UI_EVENTS)
            Serial.println("[CHILD] Clic ignore (etait un geste)");
        return;
    }

    switch (currentChild)
    {
    case UiChild::UiChildTrackImage:
        if (DEBUG_UI_EVENTS)
            Serial.println("[CHILD] TrackImage clicked");
        break;
    case UiChild::UiChildTrackInfos:
        action_play_pause_btn(nullptr);
        if (DEBUG_UI_EVENTS)
            Serial.println("[CHILD] TrackInfos clicked");
        break;
    default:
        if (DEBUG_UI_EVENTS)
            Serial.println("[CHILD] Unknown child clicked");
    }

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
        reportCommand(player.adjustVolume(1));
        break;

    case LV_DIR_BOTTOM:
        reportCommand(player.adjustVolume(-1));
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
            currentScreen = UIScreen::UIScreenPlayer;
            loadScreen(SCREEN_ID_UI_PLAYER);
        }
        break;
    case LV_DIR_RIGHT:
        if (currentScreen == UIScreen::UIScreenPlayer)
        {
            currentScreen = UIScreen::UIScreenBrowser;
            loadScreen(SCREEN_ID_UI_BROWSER);
        }
        break;
    default:
        break;
    }
}

void action_gesture(lv_event_t *e)
{
    s_gestureHandled = true; // un swipe vient d'etre traite : le clic qui suivra sera ignore

    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (currentScreen == UIScreen::UIScreenBrowser)
    {
        handle_browser_gesture(dir);
    }
    else if (currentScreen == UIScreen::UIScreenPlayer)
    {
        // For Player screen
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
}
