#include "audio_player.h"
#include <SD_MMC.h>
#include "playback/jukebox.h"

// Broches I2S vers le DAC/ampli (4, 10, 11 sont libres sur cette carte,
// 33-37 sont reserves en interne au PSRAM Octal)
#define I2S_BCLK 4
#define I2S_LRC 10
#define I2S_DOUT 11

bool AudioPlayer::begin()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(12); // 0..21

    // Lambda capturant "this" : AudioPlayer est un objet global de longue duree,
    // donc ce pointeur reste valide pendant toute l'execution.
    Audio::audio_info_callback = [this](Audio::msg_t m)
    { this->onInfo(m); };

    return true;
}

void AudioPlayer::loop()
{
    audio.loop();
}

void AudioPlayer::attachJukebox(Jukebox &jb)
{
    jukebox = &jb;
}

void AudioPlayer::playCurrent()
{
    if (!jukebox)
        return;

    const Music *track = jukebox->current();
    if (!track || !track->isValid())
    {
        Serial.println("[AudioPlayer] Piste invalide, lecture annulee.");
        return;
    }

    Serial.printf("\n>>> Lecture : %s\n", track->path.c_str());
    Serial.printf("[Debug] Heap libre: %u | PSRAM libre: %u\n",
                  (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

    bool ok = audio.connecttoFS(SD_MMC, track->path.c_str());
    Serial.printf("[Debug] connecttoFS() -> %s\n", ok ? "OK" : "ECHEC");
    Serial.printf("[Debug] isRunning() juste apres -> %s\n", audio.isRunning() ? "true" : "false");
}

void AudioPlayer::next()
{
    if (!jukebox)
        return;
    if (jukebox->next())
        playCurrent();
}

void AudioPlayer::previous()
{
    if (!jukebox)
        return;
    if (jukebox->previous())
        playCurrent();
}

void AudioPlayer::pauseResume()
{
    Serial.printf("[Debug] pauseResume() : isRunning() avant = %s\n", audio.isRunning() ? "true" : "false");
    audio.pauseResume();
    Serial.printf("[Debug] pauseResume() : isRunning() apres = %s\n", audio.isRunning() ? "true" : "false");
}

bool AudioPlayer::isRunning()
{
    return audio.isRunning();
}

void AudioPlayer::setVolume(uint8_t vol)
{
    audio.setVolume(vol);
}

uint8_t AudioPlayer::getVolume()
{
    return audio.getVolume();
}

uint32_t AudioPlayer::currentTime()
{
    return audio.getAudioCurrentTime();
}

uint32_t AudioPlayer::duration()
{
    return audio.getAudioFileDuration();
}

void AudioPlayer::seekTo(uint32_t targetSeconds)
{
    if (!audio.isRunning())
        return;

    int32_t cur = (int32_t)audio.getAudioCurrentTime();
    int32_t offset = (int32_t)targetSeconds - cur;

    if (offset == 0)
        return; // deja a la bonne position, rien a faire

    bool ok = audio.setTimeOffset(offset);
    Serial.printf("[Debug] seekTo(%u) : offset=%d -> %s\n",
                  (unsigned)targetSeconds, (int)offset, ok ? "OK" : "ECHEC");
}

void AudioPlayer::onInfo(Audio::msg_t m)
{
    if (!m.msg)
        return;
    String txt = m.msg;

    switch (m.e)
    {
    case Audio::evt_id3data:
        if (jukebox)
        {
            if (txt.startsWith("Title: "))
                jukebox->setCurrentTitle(txt.substring(7));
            else if (txt.startsWith("Artist: "))
                jukebox->setCurrentArtist(txt.substring(8));
            else if (txt.startsWith("Album: "))
                jukebox->setCurrentAlbum(txt.substring(7));
        }
        Serial.printf("[ID3] %s\n", txt.c_str());
        break;

    case Audio::evt_eof:
        Serial.println("[EOF] Fin de piste -> suivante");
        next();
        break;

    case Audio::evt_info:
    case Audio::evt_bitrate:
        Serial.printf("[INFO] %s\n", txt.c_str());
        break;

    case Audio::evt_log:
        Serial.printf("[LOG] %s\n", txt.c_str());
        break;

    default:
        Serial.printf("[Autre evt=%d] %s\n", (int)m.e, txt.c_str());
        break;
    }
}