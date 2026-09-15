#pragma once
#include <Arduino.h>
#include <Audio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "playback/jukebox.h"

class AudioPlayer
{
public:
    // Copie coherente, sans pointeur vers le Jukebox ou le decodeur.
    struct State
    {
        bool running = false;
        uint8_t volume = 12;
        uint32_t currentTime = 0;
        uint32_t duration = 0;
        uint32_t trackGeneration = 0;
        uint32_t seekRequest = 0;
        bool seekAccepted = false;
        bool hasTrack = false;
        String title, artist, album;
        bool shuffle = false;
        RepeatMode repeat = RepeatMode::OFF;
    };

    AudioPlayer() = default;
    AudioPlayer(const AudioPlayer &) = delete;
    AudioPlayer &operator=(const AudioPlayer &) = delete;
    // Dans setup(), avant LVGL. Objet global de longue duree.
    bool begin();
    // Exclusivement depuis la tache Arduino, proprietaire du decodeur.
    void loop();
    // Dans setup(), apres chargement de la bibliotheque, avant le premier loop().
    // Ensuite, aucun acces direct au Jukebox depuis une autre tache.
    void attachJukebox(Jukebox &jb);

    // Asynchrones : true = accepte, pas encore execute ; false = file pleine
    // ou player non initialise. Les envois ne bloquent jamais LVGL.
    bool playCurrent();
    bool next();
    bool previous();
    bool previousOrRestart();
    bool pauseResume();
    bool setVolume(uint8_t vol);
    bool adjustVolume(int8_t delta);
    bool seekTo(uint32_t targetSeconds, uint32_t request = 0);
    bool toggleShuffle();
    bool cycleRepeatMode();

    State state();
    bool isRunning() { return state().running; }
    uint8_t getVolume() { return state().volume; }
    uint32_t currentTime() { return state().currentTime; }
    uint32_t duration() { return state().duration; }

private:
    enum class CommandType : uint8_t
    {
        Play, Next, Previous, PreviousOrRestart, PauseResume,
        SetVolume, AdjustVolume, Seek, Shuffle, Repeat
    };
    // FreeRTOS copie les valeurs : ni String ni pointeur dans la file.
    struct Command { CommandType type; int64_t value; uint32_t request; };
    static constexpr UBaseType_t CommandCapacity = 24;
    Audio audio;
    Jukebox *jukebox = nullptr;
    QueueHandle_t _commands = nullptr;
    SemaphoreHandle_t _stateMutex = nullptr;
    State _state;
    bool _trackEnded = false;
    bool _discardTrackEvents = false;
    uint32_t _lastPublish = 0;
    uint32_t _trackGeneration = 0;
    uint32_t _seekRequest = 0;
    bool _seekAccepted = false;

    bool enqueue(CommandType type, int64_t value = 0, uint32_t request = 0);
    void execute(const Command &command);
    void playCurrentNow();
    void nextNow();
    bool seekNow(uint32_t targetSeconds);
    void publishState();
    void onInfo(Audio::msg_t m);
};
