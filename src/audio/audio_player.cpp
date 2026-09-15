#include "audio_player.h"
#include <SD_MMC.h>

#define I2S_BCLK 4
#define I2S_LRC 10
#define I2S_DOUT 11

bool AudioPlayer::begin()
{
    if (_commands)
        return true;
    _stateMutex = xSemaphoreCreateMutex();
    if (!_stateMutex)
        return false;
    _commands = xQueueCreate(CommandCapacity, sizeof(Command));
    if (!_commands)
    {
        vSemaphoreDelete(_stateMutex);
        _stateMutex = nullptr;
        return false;
    }
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(12);
    Audio::audio_info_callback = [this](Audio::msg_t m) { onInfo(m); };
    publishState();
    return true;
}

bool AudioPlayer::enqueue(CommandType type, int64_t value, uint32_t request)
{
    const Command command{type, value, request};
    return _commands && xQueueSendToBack(_commands, &command, 0) == pdTRUE;
}

bool AudioPlayer::playCurrent() { return enqueue(CommandType::Play); }
bool AudioPlayer::next() { return enqueue(CommandType::Next); }
bool AudioPlayer::previous() { return enqueue(CommandType::Previous); }
bool AudioPlayer::previousOrRestart() { return enqueue(CommandType::PreviousOrRestart); }
bool AudioPlayer::pauseResume() { return enqueue(CommandType::PauseResume); }
bool AudioPlayer::setVolume(uint8_t vol) { return enqueue(CommandType::SetVolume, vol); }
bool AudioPlayer::adjustVolume(int8_t delta) { return enqueue(CommandType::AdjustVolume, delta); }
bool AudioPlayer::seekTo(uint32_t seconds, uint32_t request) { return enqueue(CommandType::Seek, seconds, request); }
bool AudioPlayer::toggleShuffle() { return enqueue(CommandType::Shuffle); }
bool AudioPlayer::cycleRepeatMode() { return enqueue(CommandType::Repeat); }

void AudioPlayer::attachJukebox(Jukebox &jb)
{
    jukebox = &jb;
}

void AudioPlayer::loop()
{
    if (!_commands)
        return;

    // Livrer les evenements de la piste precedente avant toute commande
    // qui pourrait changer le Jukebox (notamment les tags ID3 en attente).
    audio.loop();
    // Le callback ne rappelle jamais connecttoFS() : get_info() detient
    // un mutex interne a la lib. EOF est traite apres le retour de loop().
    if (_trackEnded)
    {
        _trackEnded = false;
        nextNow();
    }
    else
    {
        // Une operation par tour ; au plus quatre messages regroupes pour
        // Seek/SetVolume consecutifs, sans depasser une commande differente.
        Command command{}, following{};
        if (xQueueReceive(_commands, &command, 0) == pdTRUE)
        {
            for (unsigned i = 1; i < 4 &&
                 (command.type == CommandType::Seek || command.type == CommandType::SetVolume) &&
                 xQueuePeek(_commands, &following, 0) == pdTRUE && following.type == command.type; ++i)
                xQueueReceive(_commands, &command, 0);
            execute(command);
        }
    }
    // Limiter les copies de String ; l'UI se rafraichit toutes les 100 ms.
    if (millis() - _lastPublish >= 50)
    {
        _lastPublish = millis();
        publishState();
    }
}

void AudioPlayer::execute(const Command &command)
{
    switch (command.type)
    {
    case CommandType::Play: playCurrentNow(); break;
    case CommandType::Next: nextNow(); break;
    case CommandType::PreviousOrRestart:
        // Decision sur le temps reel au moment de l'execution.
        if (audio.getAudioCurrentTime() > 3)
        {
            seekNow(0);
            break;
        }
        // Sinon, meme comportement que Previous.
        [[fallthrough]];
    case CommandType::Previous:
        if (jukebox && jukebox->previous()) playCurrentNow();
        break;
    case CommandType::PauseResume: audio.pauseResume(); break;
    case CommandType::SetVolume:
        audio.setVolume((uint8_t)constrain(command.value, 0LL, 21LL));
        break;
    case CommandType::AdjustVolume:
        audio.setVolume((uint8_t)constrain((int64_t)audio.getVolume() + command.value, 0LL, 21LL));
        break;
    case CommandType::Seek:
        _seekAccepted = seekNow((uint32_t)command.value);
        _seekRequest = command.request;
        break;
    case CommandType::Shuffle:
        if (jukebox) jukebox->toggleShuffle();
        break;
    case CommandType::Repeat:
        if (jukebox) jukebox->cycleRepeatMode();
        break;
    }
}

void AudioPlayer::playCurrentNow()
{
    const Music *track = jukebox ? jukebox->current() : nullptr;
    if (!track || !track->isValid())
        return;
    ++_trackGeneration;
    // La lib livre ses infos au DEBUT du prochain loop(). Vider les infos
    // encore en attente apres l'arret, sans appliquer les anciens tags/EOF
    // au nouveau morceau deja selectionne dans le Jukebox.
    _discardTrackEvents = true;
    audio.stopSong();
    audio.loop();
    _discardTrackEvents = false;
    _trackEnded = false;
    bool ok = audio.connecttoFS(SD_MMC, track->path.c_str());
    Serial.printf("[AudioPlayer] Lecture %s : %s\n", track->path.c_str(), ok ? "OK" : "ECHEC");
}

void AudioPlayer::nextNow()
{
    if (jukebox && jukebox->next())
        playCurrentNow();
}

bool AudioPlayer::seekNow(uint32_t targetSeconds)
{
    if (!audio.isRunning())
        return false;
    uint32_t total = audio.getAudioFileDuration();
    if (total && targetSeconds > total)
        targetSeconds = total;
    // La lib supporte un temps absolu. Un offset relatif s'appuie sur une
    // position qui peut encore correspondre au seek precedent en traitement.
    if (targetSeconds > UINT16_MAX)
        return false;
    return audio.setAudioPlayTime((uint16_t)targetSeconds);
}

AudioPlayer::State AudioPlayer::state()
{
    if (!_stateMutex)
        return State{};
    xSemaphoreTake(_stateMutex, portMAX_DELAY);
    State copy = _state;
    xSemaphoreGive(_stateMutex);
    return copy;
}

void AudioPlayer::publishState()
{
    State next;
    next.running = audio.isRunning();
    next.volume = audio.getVolume();
    next.currentTime = audio.getAudioCurrentTime();
    next.duration = audio.getAudioFileDuration();
    next.trackGeneration = _trackGeneration;
    next.seekRequest = _seekRequest;
    next.seekAccepted = _seekAccepted;
    if (jukebox)
    {
        next.shuffle = jukebox->isShuffleEnabled();
        next.repeat = jukebox->repeatMode();
        const Music *track = jukebox->current();
        if (track)
        {
            next.hasTrack = true;
            next.title = track->title;
            next.artist = track->artist;
            next.album = track->album;
        }
    }
    // Ce verrou ne couvre aucun appel audio, SD ou LVGL.
    xSemaphoreTake(_stateMutex, portMAX_DELAY);
    _state = next;
    xSemaphoreGive(_stateMutex);
}

void AudioPlayer::onInfo(Audio::msg_t m)
{
    // Dans la version epinglee de la lib, get_info() livre les evenements
    // depuis audio.loop(). Le Jukebox reste donc dans la tache proprietaire.
    if (_discardTrackEvents && (m.e == Audio::evt_eof || m.e == Audio::evt_id3data))
        return;
    if (m.e == Audio::evt_eof)
    {
        _trackEnded = true;
        return;
    }
    if (!m.msg)
        return;
    if (m.e == Audio::evt_id3data && jukebox)
    {
        String txt = m.msg;
        if (txt.startsWith("Title: ")) jukebox->setCurrentTitle(txt.substring(7));
        else if (txt.startsWith("Artist: ")) jukebox->setCurrentArtist(txt.substring(8));
        else if (txt.startsWith("Album: ")) jukebox->setCurrentAlbum(txt.substring(7));
    }
    Serial.printf("[Audio evt=%d] %s\n", (int)m.e, m.msg);
}
