#pragma once
#include <Arduino.h>
#include <Audio.h>

class Jukebox;

// Fait le lien entre la lib ESP32-audioI2S (decodage + sortie I2S) et le Jukebox
// (file de lecture). Le Jukebox ne sait pas jouer du son, l'AudioPlayer ne sait
// pas gerer une file : chacun son role.
class AudioPlayer
{
public:
    // Configure les broches I2S et le callback d'infos audio (ID3, fin de piste...).
    // A appeler une fois dans setup(), avant l'init de l'ecran/LVGL (voir main.cpp).
    bool begin();

    // A appeler a chaque loop() pour laisser la lib decoder/jouer l'audio.
    void loop();

    // Associe le Jukebox dont ce player doit suivre la piste courante.
    void attachJukebox(Jukebox &jb);

    // Lance la lecture de jukebox->current()
    void playCurrent();

    // Passe a la piste suivante/precedente du Jukebox et la joue.
    void next();
    void previous();

    void pauseResume();
    bool isRunning();

    void setVolume(uint8_t vol); // 0..21
    uint8_t getVolume();

    uint32_t currentTime(); // secondes ecoulees sur la piste en cours
    uint32_t duration();    // duree totale de la piste en cours (secondes)

    // Deplace la lecture a une position absolue (en secondes) dans la piste
    // en cours. La lib sous-jacente ne fait que des sauts relatifs
    // (setTimeOffset), donc on calcule l'ecart necessaire ici.
    void seekTo(uint32_t targetSeconds);

private:
    Audio audio;
    Jukebox *jukebox = nullptr;

    void onInfo(Audio::msg_t m);
};