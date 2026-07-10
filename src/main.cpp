#include <Arduino.h>
#include <esp_system.h>
#include "Button.h"

#define BOOT_PIN 0  // GPIO0, bouton BOOT

Button bootButton(BOOT_PIN, true, true); // bouton BOOT actif à l'état bas

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  ets_printf("Appuyez sur le bouton BOOT pour redémarrer le microcontrôleur.\n");
  bootButton.onLongPress([=]()  { esp_restart(); }); // Appel de la fonction de soft reset sur appui du bouton
  bootButton.begin();
}

void loop()
{
  ets_printf("%lu\r\n", millis());
  ets_delay_us(1000000); // 1 seconde
}