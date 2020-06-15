#include <EEPROM.h>

#define USE_NEOPIXELBUS

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

#ifdef USE_ADAFRUIT_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#define LED_STRIP_TYP (NEO_GRBW + NEO_KHZ800)
Adafruit_NeoPixel *strip;
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> *strip;
#endif // USE_NEOPIXELBUS


LED_Streifen::LED_Streifen() {
  _n_Pixel = 0;
}


void LED_Streifen::Beginn(uint16_t n_Leds, uint8_t brightness) {
  _n_Pixel = n_Leds;
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip = new Adafruit_NeoPixel(1024, LED_PIN, LED_STRIP_TYP);
  strip->begin();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(512, LED_PIN);
  strip->Begin();
#endif // USE_NEOPIXELBUS
  Show();
  delay(1);
  delete strip;
  D_PRINT("strip (0)");

#ifdef USE_ADAFRUIT_NEOPIXEL
  strip = new Adafruit_NeoPixel(n_Leds, LED_PIN, LED_STRIP_TYP);
  strip->begin();
  strip->setBrightness(brightness);
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(n_Leds, LED_PIN);
  strip->Begin();
#endif // USE_NEOPIXELBUS
  Show();
  D_PRINTLN("ENDE LED_Streifen");
}


void LED_Streifen::Bereit() {
}


void LED_Streifen::MonoFarbe(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t erste) {
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip->fill(strip->Color(0, 0, 0, strip->gamma8(_Helligkeit)), erste, _n_Pixel - erste);
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip->ClearTo(RgbwColor(r, g, b, w), erste, _n_Pixel - 1);
#endif // USE_NEOPIXELBUS
}


void LED_Streifen::SetPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip->setPixelColor(n, strip->Color(r, g, b, w));
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip->SetPixelColor(n, RgbwColor(r, g, b, w));
#endif // USE_NEOPIXELBUS
}


void LED_Streifen::Show() {
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip->Show();
#endif // USE_NEOPIXELBUS
}
