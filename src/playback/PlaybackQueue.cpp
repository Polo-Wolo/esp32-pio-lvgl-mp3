#include "PlaybackQueue.h"


void PlaybackQueue::add(const MusicTrack& track)
{
    if (!track.isValid())
    {
        return;
    }

    tracks.push_back(track);
}


void PlaybackQueue::add(const std::vector<MusicTrack>& newTracks)
{
    for (const MusicTrack& track : newTracks)
    {
        add(track);
    }
}


bool PlaybackQueue::insert(size_t index, const MusicTrack& track)
{
    if (!track.isValid())
    {
        return false;
    }

    if (index > tracks.size())
    {
        return false;
    }

    tracks.insert(tracks.begin() + index, track);

    return true;
}


bool PlaybackQueue::remove(size_t index)
{
    if (index >= tracks.size())
    {
        return false;
    }

    tracks.erase(tracks.begin() + index);

    return true;
}


void PlaybackQueue::clear()
{
    tracks.clear();
}


size_t PlaybackQueue::size() const
{
    return tracks.size();
}


bool PlaybackQueue::empty() const
{
    return tracks.empty();
}


MusicTrack* PlaybackQueue::get(size_t index)
{
    if (index >= tracks.size())
    {
        return nullptr;
    }

    return &tracks[index];
}


const MusicTrack* PlaybackQueue::get(size_t index) const
{
    if (index >= tracks.size())
    {
        return nullptr;
    }

    return &tracks[index];
}


MusicTrack* PlaybackQueue::first()
{
    if (tracks.empty())
    {
        return nullptr;
    }

    return &tracks.front();
}


const MusicTrack* PlaybackQueue::first() const
{
    if (tracks.empty())
    {
        return nullptr;
    }

    return &tracks.front();
}


MusicTrack* PlaybackQueue::last()
{
    if (tracks.empty())
    {
        return nullptr;
    }

    return &tracks.back();
}


const MusicTrack* PlaybackQueue::last() const
{
    if (tracks.empty())
    {
        return nullptr;
    }

    return &tracks.back();
}