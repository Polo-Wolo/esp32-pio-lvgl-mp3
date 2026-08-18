#include "playback_manager.h"

PlaybackManager::PlaybackManager()
    : _queueSize(0),
      _currentIndex(-1)
{
}

// ==================================================
// QUEUE
// ==================================================

bool PlaybackManager::add(const MusicTrack &track)
{
    if (_queueSize >= MAX_QUEUE_SIZE)
    {
        return false;
    }

    if (!track.isValid())
    {
        return false;
    }

    _queue[_queueSize] = track;
    _queueSize++;

    // Le premier morceau devient automatiquement
    // le morceau courant.
    if (_currentIndex == -1)
    {
        _currentIndex = 0;
    }

    return true;
}

void PlaybackManager::clear()
{
    _queueSize = 0;
    _currentIndex = -1;
}

size_t PlaybackManager::size() const
{
    return _queueSize;
}

bool PlaybackManager::empty() const
{
    return _queueSize == 0;
}

// ==================================================
// CURRENT
// ==================================================

const MusicTrack *PlaybackManager::current() const
{
    if (_currentIndex < 0)
    {
        return nullptr;
    }

    if (_currentIndex >= static_cast<int>(_queueSize))
    {
        return nullptr;
    }

    return &_queue[_currentIndex];
}

// ==================================================
// NEXT
// ==================================================

const MusicTrack *PlaybackManager::next()
{
    if (_queueSize == 0)
    {
        return nullptr;
    }

    if (_currentIndex + 1 >= static_cast<int>(_queueSize))
    {
        return nullptr;
    }

    _currentIndex++;

    return &_queue[_currentIndex];
}

// ==================================================
// PREVIOUS
// ==================================================

const MusicTrack *PlaybackManager::previous()
{
    if (_queueSize == 0)
    {
        return nullptr;
    }

    if (_currentIndex <= 0)
    {
        return nullptr;
    }

    _currentIndex--;

    return &_queue[_currentIndex];
}

// ==================================================
// HAS NEXT / PREVIOUS
// ==================================================

bool PlaybackManager::hasNext() const
{
    if (_queueSize == 0)
    {
        return false;
    }

    return _currentIndex + 1 < static_cast<int>(_queueSize);
}

bool PlaybackManager::hasPrevious() const
{
    if (_queueSize == 0)
    {
        return false;
    }

    return _currentIndex > 0;
}

// ==================================================
// POSITION
// ==================================================

int PlaybackManager::currentIndex() const
{
    return _currentIndex;
}

bool PlaybackManager::setCurrent(size_t index)
{
    if (index >= _queueSize)
    {
        return false;
    }

    _currentIndex = static_cast<int>(index);

    return true;
}

// ==================================================
// DEBUG
// ==================================================

void PlaybackManager::printQueue() const
{
    Serial.println();
    Serial.println("===== PLAYBACK QUEUE =====");

    for (size_t i = 0; i < _queueSize; i++)
    {
        Serial.printf(
            "%s %d - %s - %s - %s\n",
            (i == static_cast<size_t>(_currentIndex)) ? ">" : " ",
            static_cast<int>(i + 1),
            _queue[i].artist.c_str(),
            _queue[i].title.c_str(),
            _queue[i].album.c_str());
    }

    Serial.printf(
        "Current index: %d\n",
        _currentIndex);

    Serial.println("==========================");
}