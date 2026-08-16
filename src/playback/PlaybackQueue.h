#ifndef PLAYBACK_QUEUE_H
#define PLAYBACK_QUEUE_H

#include <Arduino.h>
#include <vector>

#include "MusicTrack.h"

class PlaybackQueue
{
public:

    // Ajouter un morceau à la fin
    void add(const MusicTrack& track);

    // Ajouter plusieurs morceaux à la fin
    void add(const std::vector<MusicTrack>& tracks);

    // Insérer un morceau à une position donnée
    bool insert(size_t index, const MusicTrack& track);

    // Supprimer un morceau
    bool remove(size_t index);

    // Vider complètement la queue
    void clear();

    // Nombre de morceaux
    size_t size() const;

    // Vérifie si la queue est vide
    bool empty() const;

    // Accéder à un morceau
    MusicTrack* get(size_t index);
    const MusicTrack* get(size_t index) const;

    // Premier morceau
    MusicTrack* first();
    const MusicTrack* first() const;

    // Dernier morceau
    MusicTrack* last();
    const MusicTrack* last() const;

private:

    std::vector<MusicTrack> tracks;
};

#endif