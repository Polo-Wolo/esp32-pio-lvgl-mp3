#ifndef MUSIC_TRACK_H
#define MUSIC_TRACK_H

#include <Arduino.h>

struct MusicTrack
{
    String path;
    String title;
    String artist;
    String album;
    uint16_t trackNumber = 0;

    MusicTrack() = default;

    MusicTrack(
        const String& path,
        const String& title,
        const String& artist,
        const String& album,
        uint16_t trackNumber = 0)
        : path(path),
          title(title),
          artist(artist),
          album(album),
          trackNumber(trackNumber)
    {
    }

    bool isValid() const
    {
        return !path.isEmpty();
    }
};

#endif