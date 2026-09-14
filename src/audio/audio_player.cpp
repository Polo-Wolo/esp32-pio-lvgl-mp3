#include "audio_player.h"
#include <SD_MMC.h>
#include "playback/jukebox.h"

// Broches I2S vers le DAC/ampli (4, 10, 11 sont libres sur cette carte,
// 33-37 sont reserves en interne au PSRAM Octal)
#define I2S_BCLK 4
#define I2S_LRC 10
#define I2S_DOUT 11

void AudioPlayer::lock()
{
    if (_mutex)
        xSemaphoreTakeRecursive(_mutex, portMAX_DELAY);
}

void AudioPlayer::unlock()
{
    if (_mutex)
        xSemaphoreGiveRecursive(_mutex);
}

bool AudioPlayer::begin()
{
    _mutex = xSemaphoreCreateRecursiveMutex();

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
    lock();
    audio.loop();
    unlock();

    // Traite la fin de piste EN DEHORS du callback audio et en dehors du
    // lock ci-dessus (next() reprend son propre lock plus bas).
    if (_trackEnded)
    {
        _trackEnded = false;
        next();
    }
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

    lock();
    Serial.printf("\n>>> Lecture : %s\n", track->path.c_str());
    Serial.printf("[Debug] Heap libre: %u | PSRAM libre: %u\n",
                  (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram());

    bool ok = audio.connecttoFS(SD_MMC, track->path.c_str());
    Serial.printf("[Debug] connecttoFS() -> %s\n", ok ? "OK" : "ECHEC");
    Serial.printf("[Debug] isRunning() juste apres -> %s\n", audio.isRunning() ? "true" : "false");
    unlock();
}

void AudioPlayer::next()
{
    if (!jukebox)
        return;
    lock();
    if (jukebox->next())
        playCurrent();
    unlock();
}

void AudioPlayer::previous()
{
    if (!jukebox)
        return;
    lock();
    if (jukebox->previous())
        playCurrent();
    unlock();
}

void AudioPlayer::pauseResume()
{
    lock();
    Serial.printf("[Debug] pauseResume() : isRunning() avant = %s\n", audio.isRunning() ? "true" : "false");
    audio.pauseResume();
    Serial.printf("[Debug] pauseResume() : isRunning() apres = %s\n", audio.isRunning() ? "true" : "false");
    unlock();
}

bool AudioPlayer::isRunning()
{
    lock();
    bool running = audio.isRunning();
    unlock();
    return running;
}

void AudioPlayer::setVolume(uint8_t vol)
{
    lock();
    audio.setVolume(vol);
    unlock();
}

uint8_t AudioPlayer::getVolume()
{
    lock();
    uint8_t vol = audio.getVolume();
    unlock();
    return vol;
}

uint32_t AudioPlayer::currentTime()
{
    lock();
    uint32_t t = audio.getAudioCurrentTime();
    unlock();
    return t;
}

uint32_t AudioPlayer::duration()
{
    lock();
    uint32_t d = audio.getAudioFileDuration();
    unlock();
    return d;
}

void AudioPlayer::seekTo(uint32_t targetSeconds)
{
    lock();
    if (audio.isRunning())
    {
        int32_t cur = (int32_t)audio.getAudioCurrentTime();
        int32_t offset = (int32_t)targetSeconds - cur;

        if (offset != 0)
        {
            bool ok = audio.setTimeOffset(offset);
            Serial.printf("[Debug] seekTo(%u) : offset=%d -> %s\n",
                          (unsigned)targetSeconds, (int)offset, ok ? "OK" : "ECHEC");
        }
    }
    unlock();
}

void AudioPlayer::onInfo(Audio::msg_t m)
{
    // Appelee de maniere synchrone depuis l'interieur de audio.loop() (donc
    // deja sous le lock pris par loop()). On ne doit JAMAIS rappeler
    // connecttoFS()/next() directement ici : la lib peut deadlocker en
    // interne si on la rappelle depuis son propre callback. On se contente
    // de poser un drapeau, traite ensuite dans loop() une fois le lock relache.
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
        Serial.println("[EOF] Fin de piste -> suivante (differee dans loop())");
        _trackEnded = true;
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