#include "jukebox.h"

Jukebox::Jukebox()
    : _queueSize(0),
      _currentIndex(-1)
{
}

// ==================================================
// QUEUE
// ==================================================

bool Jukebox::add(const Music &track)
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

void Jukebox::clear()
{
    _queueSize = 0;
    _currentIndex = -1;
}

size_t Jukebox::size() const
{
    return _queueSize;
}

bool Jukebox::empty() const
{
    return _queueSize == 0;
}

// ==================================================
// CURRENT
// ==================================================

const Music *Jukebox::current() const
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

const Music *Jukebox::next()
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

const Music *Jukebox::previous()
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

bool Jukebox::hasNext() const
{
    if (_queueSize == 0)
    {
        return false;
    }

    return _currentIndex + 1 < static_cast<int>(_queueSize);
}

bool Jukebox::hasPrevious() const
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

int Jukebox::currentIndex() const
{
    return _currentIndex;
}

bool Jukebox::setCurrent(size_t index)
{
    if (index >= _queueSize)
    {
        return false;
    }

    _currentIndex = static_cast<int>(index);

    return true;
}

// ==================================================
// TAGS DE LA PISTE EN COURS
// ==================================================

void Jukebox::setCurrentTitle(const String &title)
{
    if (_currentIndex < 0 || _currentIndex >= static_cast<int>(_queueSize)) return;
    _queue[_currentIndex].title = title;
}

void Jukebox::setCurrentArtist(const String &artist)
{
    if (_currentIndex < 0 || _currentIndex >= static_cast<int>(_queueSize)) return;
    _queue[_currentIndex].artist = artist;
}

void Jukebox::setCurrentAlbum(const String &album)
{
    if (_currentIndex < 0 || _currentIndex >= static_cast<int>(_queueSize)) return;
    _queue[_currentIndex].album = album;
}

// ==================================================
// DEBUG
// ==================================================

void Jukebox::printQueue() const
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
