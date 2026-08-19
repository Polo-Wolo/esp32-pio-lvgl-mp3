#include <Arduino.h>

#include "playback/jukebox.h"
#include "sd/music_library.h"
#include "audio/audio_player.h"

Jukebox     playback;
AudioPlayer player;

String fmtTime(uint32_t s)
{
    char buf[8];
    sprintf(buf, "%02u:%02u", s / 60, s % 60);
    return String(buf);
}

void handleKey(char c)
{
    switch (c)
    {
        case ' ':
            player.pauseResume();
            Serial.println(player.isRunning() ? "[Lecture]" : "[Pause]");
            break;

        case 'n': case 'N':
            player.next();
            break;

        case 'p': case 'P':
            player.previous();
            break;

        case '+': case '=':
            if (player.getVolume() < 21) player.setVolume(player.getVolume() + 1);
            Serial.printf("[Volume] %d/21\n", player.getVolume());
            break;

        case '-': case '_':
            if (player.getVolume() > 0) player.setVolume(player.getVolume() - 1);
            Serial.printf("[Volume] %d/21\n", player.getVolume());
            break;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("===== CHARGEMENT DE LA BIBLIOTHEQUE SD =====");

    if (!MusicLibrary::begin())
    {
        Serial.println("Echec montage carte SD !");
        return;
    }
    Serial.println("Carte SD montee.");

    size_t added = MusicLibrary::loadFolder(playback, "/", true);
    Serial.printf("%d piste(s) chargee(s) depuis /Music\n", (int)added);

    if (playback.empty())
    {
        Serial.println("Aucune piste trouvee. Verifiez le chemin et le contenu de la carte SD.");
        return;
    }

    player.begin();
    player.attachJukebox(playback);
    player.playCurrent();

    Serial.println();
    Serial.println("Commandes : espace=pause/lecture | n/p=piste suivante/precedente | +/-=volume");
}

void loop()
{
    player.loop();

    while (Serial.available())
    {
        handleKey((char)Serial.read());
    }

    static uint32_t lastPrint = 0;
    if (player.isRunning() && millis() - lastPrint > 1000)
    {
        lastPrint = millis();
        const Music *track = playback.current();
        Serial.printf("\r[%s / %s] %s - %s          ",
                       fmtTime(player.currentTime()).c_str(),
                       fmtTime(player.duration()).c_str(),
                       track ? track->artist.c_str() : "?",
                       track ? track->title.c_str() : "?");
    }

    vTaskDelay(1);
}
