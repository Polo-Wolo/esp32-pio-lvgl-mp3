#pragma once
#include <stdint.h>

// Filtre d'affichage seulement : ne change jamais la duree du decodeur audio.
class DurationDisplay {
public:
    void reset(uint32_t now, uint32_t initial = 0) {
        value = candidate = initial;
        started = since = now;
    }
    uint32_t update(uint32_t raw, uint32_t now) {
        if (!raw) { candidate = 0; since = now; return value; }
        const uint32_t difference = raw > candidate ? raw - candidate : candidate - raw;
        if (!candidate || difference > 2) { candidate = raw; since = now; }
        // Accepter une estimation apres 1,5 s dans une bande de +/-2 s,
        // au plus tot 2 s apres le changement de piste. Aucune duree figee
        // definitivement : une correction ulterieure doit se stabiliser aussi.
        if (uint32_t(now - started) >= 2000 && uint32_t(now - since) >= 1500) {
            const uint32_t change = raw > value ? raw - value : value - raw;
            if (!value || change > 2) value = raw;
        }
        return value;
    }
private:
    uint32_t value = 0, candidate = 0, started = 0, since = 0;
};
