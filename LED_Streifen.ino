#include <EEPROM.h>

#include "main.h"
#include "LED_Streifen.h"

// Pin fuer die NeoPixels

#ifdef IST_ESP01
//#define LED_PIN     2 // RX
#define LED_PIN     1 // TX
#endif // IST_ESP01
#ifdef IST_SONOFF // Achtung: nicht allgemein für SONOFF gültig
#define LED_PIN     3
#endif // IST_SONOFF
#ifdef IST_NODEMCU32
#define LED_PIN     27 // 14
#endif // IST_NODEMCU32
#ifdef IST_ESP32S
#define LED_PIN     13
#endif // IST_ESP32S

#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> *strip;


LED_Streifen::LED_Streifen() {
  _n_Pixel = 0;
}


void LED_Streifen::Beginn(uint16_t n_Leds) {
  _n_Pixel = n_Leds;
  strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(512, LED_PIN);
  strip->Begin();
  Show();
  delay(1);
  delete strip;
  D_PRINT("strip (0)");

  strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(n_Leds, LED_PIN);
  strip->Begin();
  Show();
  D_PRINTLN("ENDE LED_Streifen");
}


void LED_Streifen::Bereit() {
}


void LED_Streifen::MonoFarbe(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t erste) {
  strip->ClearTo(RgbwColor(r, g, b, w), erste, _n_Pixel - 1);
}


void LED_Streifen::SetPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  strip->SetPixelColor(n, RgbwColor(r, g, b, w));
}


void LED_Streifen::Show() {
  strip->Show();
}
