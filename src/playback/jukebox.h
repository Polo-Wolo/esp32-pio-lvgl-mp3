#pragma once
#include <Arduino.h>

// Taille max de la file de lecture (ajustez selon votre RAM disponible)
static const size_t MAX_QUEUE_SIZE = 200;

// ==================================================
// Music : un morceau et ses metadonnees
// ==================================================
class Music
{
public:
    String path;
    String title;
    String artist;
    String album;
    int trackNumber;

    // Constructeur par defaut : piste "vide" / invalide
    Music()
        : path(""), title(""), artist(""), album(""), trackNumber(0)
    {
    }

    Music(const String &path,
          const String &title,
          const String &artist,
          const String &album,
          int trackNumber)
        : path(path), title(title), artist(artist), album(album), trackNumber(trackNumber)
    {
    }

    bool isValid() const
    {
        return path.length() > 0;
    }

    void print() const
    {
        Serial.println("----- Piste -----");
        Serial.printf("Titre   : %s\n", title.c_str());
        Serial.printf("Artiste : %s\n", artist.c_str());
        Serial.printf("Album   : %s\n", album.c_str());
        Serial.printf("No piste: %d\n", trackNumber);
        Serial.printf("Chemin  : %s\n", path.c_str());
    }
};

// ==================================================
// Jukebox : file de lecture + position courante
// ==================================================
enum class RepeatMode
{
    OFF,
    ALL,
    ONE
};

class Jukebox
{
public:
    Jukebox();

    // Queue
    bool add(const Music &track);
    void clear();
    size_t size() const;
    bool empty() const;

    // Current
    const Music *current() const;

    // Next / previous (tiennent compte du shuffle et du repeat)
    const Music *next();
    const Music *previous();
    bool hasNext() const;
    bool hasPrevious() const;

    // Position
    int currentIndex() const;
    bool setCurrent(size_t index);

    // Met a jour les tags de la piste EN COURS (appele quand le decodeur
    // audio lit les vrais tags ID3, qui remplacent le titre provisoire
    // donne par MusicLibrary). Ne fait rien si aucune piste n'est en cours.
    void setCurrentTitle(const String &title);
    void setCurrentArtist(const String &artist);
    void setCurrentAlbum(const String &album);

    // Lecture aleatoire
    void toggleShuffle();
    bool isShuffleEnabled() const;

    // Boucle : OFF -> ALL -> ONE -> OFF a chaque appel (comportement bouton unique)
    void cycleRepeatMode();
    RepeatMode repeatMode() const;

    // Debug
    void printQueue() const;

private:
    Music _queue[MAX_QUEUE_SIZE];
    size_t _queueSize;
    int _currentIndex;

    bool _shuffle = false;
    RepeatMode _repeat = RepeatMode::OFF;
};