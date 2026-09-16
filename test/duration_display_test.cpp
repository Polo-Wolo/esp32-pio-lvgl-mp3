// Test autonome sur PC : c++ -std=c++11 test/duration_display_test.cpp -o duration_test
#include "../src/ui/duration_display.h"
#include <assert.h>

int main()
{
    DurationDisplay display;
    display.reset(0);
    assert(display.update(0, 100) == 0);
    assert(display.update(232, 200) == 0);
    assert(display.update(232, 1900) == 0);
    assert(display.update(232, 2000) == 232);
    // Ne pas faire osciller le texte avec le bruit de l'estimation.
    assert(display.update(233, 2100) == 232);
    assert(display.update(231, 2200) == 232);
    // Une correction importante reste possible, apres stabilisation.
    assert(display.update(260, 2300) == 232);
    assert(display.update(260, 3700) == 232);
    assert(display.update(260, 3800) == 260);
    // Changement de piste : ne jamais conserver la duree precedente.
    display.reset(4000);
    assert(display.update(0, 4100) == 0);
    for (uint32_t t = 4200; t < 7000; t += 100)
        assert(display.update(100 + (t - 4200) / 10, t) == 0);
    assert(display.update(306, 7000) == 0);
    assert(display.update(306, 8500) == 306);
    // millis() deborde apres environ 49 jours.
    display.reset(UINT32_MAX - 1000);
    assert(display.update(200, UINT32_MAX - 900) == 0);
    assert(display.update(200, 1100) == 200);
}
