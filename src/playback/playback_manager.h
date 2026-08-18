#ifndef PLAYBACK_MANAGER_H
#define PLAYBACK_MANAGER_H

#include <Arduino.h>
#include "music_track.h"

class PlaybackManager
{
public:
    static const size_t MAX_QUEUE_SIZE = 50;

    PlaybackManager();

    // Queue
    bool add(const MusicTrack &track);
    void clear();

    size_t size() const;
    bool empty() const;

    // Navigation
    const MusicTrack *current() const;
    const MusicTrack *next();
    const MusicTrack *previous();

    bool hasNext() const;
    bool hasPrevious() const;

    // Position
    int currentIndex() const;
    bool setCurrent(size_t index);

    // Debug
    void printQueue() const;

private:
    MusicTrack _queue[MAX_QUEUE_SIZE];

    size_t _queueSize;
    int _currentIndex;
};

#endif