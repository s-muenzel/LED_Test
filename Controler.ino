#include <EEPROM.h>

#define USE_NEOPIXELBUS

#include "main.h"
#include "Controler.h"
#include "Color_Helper.h"

#define POS_MODUS         0
#define POS_HELLIGKEIT    sizeof(_Modus)
#define POS_FARBE1        POS_HELLIGKEIT + sizeof(_Helligkeit)
#define POS_FARBE2        POS_FARBE1 + sizeof(_Farbe1)
#define POS_SPEED         POS_FARBE2 + sizeof(_Farbe2)
#define POS_N_LEDS        POS_SPEED + sizeof(_Speed)
#define POS_BRIGHTNESS    POS_N_LEDS + sizeof(_n_Leds)
#define GROESSE_ALLES     POS_BRIGHTNESS + sizeof(_Brightness)

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
// "Neue" WS2812B: 800kHz (passt auf die 18.Feier Eva LEDs)
//#define LED_STRIP_TYP (NEO_GRB + NEO_KHZ800)
// WS2812: 400 kHz
//#define LED_STRIP_TYP (NEO_GRB + NEO_KHZ400)
// "Neue" SK6812(~WS2812B) RGBW (WarmWhite):
// Led Strip SK6812(Similar WS2812B) RGBW 3m 60Leds/m IP30 DC5V
#define LED_STRIP_TYP (NEO_GRBW + NEO_KHZ800)
Adafruit_NeoPixel *strip;
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> *strip;
#endif // USE_NEOPIXELBUS

LichtModi::LichtModi() {
  PlusMinus_Mode = 0;
}

void LichtModi::Beginn() {
  D_PRINTLN("LichtModi Beginn");
  EEPROM.begin(GROESSE_ALLES);
  EEPROM.get(POS_MODUS, _Modus);
  EEPROM.get(POS_HELLIGKEIT, _Helligkeit);
  EEPROM.get(POS_FARBE1, _Farbe1); Set_Farbe1(_Farbe1);
  EEPROM.get(POS_FARBE2, _Farbe2); Set_Farbe2(_Farbe2);
  EEPROM.get(POS_SPEED, _Speed);
  EEPROM.get(POS_N_LEDS, _n_Leds);
  EEPROM.get(POS_BRIGHTNESS, _Brightness);
  if ((_n_Leds < 1) || (_n_Leds > 1024)) {
    D_PRINTLN("EEPROM nicht richtig initialisiert, initiere neu");
    _Modus = Aus;
    _Helligkeit = 64;
    _Farbe1 = 0x0f0f0f;  Set_Farbe1(_Farbe1);
    _Farbe2 = 0x0f0f0f;  Set_Farbe2(_Farbe2);
    _Speed = 10;
    _n_Leds = 1;
    _Brightness = 32;
    EEPROM.put(POS_MODUS, _Modus);
    EEPROM.put(POS_HELLIGKEIT, _Helligkeit);
    EEPROM.put(POS_FARBE1, _Farbe1);
    EEPROM.put(POS_FARBE2, _Farbe2);
    EEPROM.put(POS_SPEED, _Speed);
    EEPROM.put(POS_N_LEDS, _n_Leds);
    EEPROM.put(POS_BRIGHTNESS, _Brightness);
    EEPROM.commit();
  }
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip = new Adafruit_NeoPixel(1024, LED_PIN, LED_STRIP_TYP);
  strip->begin();
  strip->show();
  delay(1);
  delete strip;
#endif // USE_ADAFRUIT_NEOPIXEL
  D_PRINT("strip (0)");

#ifdef USE_ADAFRUIT_NEOPIXEL
  strip = new Adafruit_NeoPixel(_n_Leds, LED_PIN, LED_STRIP_TYP);
  strip->begin();
  strip->show();
  strip->setBrightness(_Brightness);
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip = new NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod>(_n_Leds, LED_PIN);
  strip->Begin();
  strip->Show();
#endif // USE_NEOPIXELBUS
  D_PRINTLN("ENDE LichtModi");
}

void LichtModi::Bereit() {
}

void LichtModi::Tick() {
  switch (_Modus) {
    default:
    case Aus:

#ifdef USE_ADAFRUIT_NEOPIXEL
      strip->fill();
      strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
      strip->ClearTo(RgbwColor(0));
      strip->Show();
#endif // USE_NEOPIXELBUS
      break;
    case Weiss:
#ifdef USE_ADAFRUIT_NEOPIXEL
      strip->fill(strip->Color(0, 0, 0, strip->gamma8(_Helligkeit)));
      strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
      strip->ClearTo(RgbwColor(_Helligkeit));
      strip->Show();
#endif // USE_NEOPIXELBUS
      break;
    case Farbe:
#ifdef USE_ADAFRUIT_NEOPIXEL
      strip->fill(_Farbe1);
      strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
      strip->ClearTo(HtmlColor(_Farbe1));
      strip->Show();
#endif // USE_NEOPIXELBUS
      break;
    case Verlauf:
      Tick_Verlauf();
      break;
    case Verlauf2:
      Tick_Verlauf2();
      break;
  }
}

void LichtModi::Set_Modus(Modi mode, bool commit) {
  _Modus = mode;
  D_PRINTF("Neuer Modus: %s\n", Get_Modus_Name());
  EEPROM.put(POS_MODUS, _Modus);
  if (commit)
    EEPROM.commit();
  PlusMinus_Mode = 0;
}

void LichtModi::Set_Modus(const char* mode, bool commit) {
  if (strcmp(mode, "Aus") == 0) {
    _Modus = Aus;
  } else if (strcmp(mode, "Weiss") == 0) {
    _Modus = Weiss;
  } else if (strcmp(mode, "Farbe") == 0) {
    _Modus = Farbe;
  } else if (strcmp(mode, "Verlauf") == 0) {
    _Modus = Verlauf;
  } else if (strcmp(mode, "Verlauf2") == 0) {
    _Modus = Verlauf2;
  } else
    _Modus = Aus;
  D_PRINTF("Neuer Modus: %s\n", Get_Modus_Name());
  EEPROM.put(POS_MODUS, _Modus);
  if (commit)
    EEPROM.commit();
  PlusMinus_Mode = 0;
}

void LichtModi::Next_Modus() {
  switch (_Modus) {
    default:
    case Aus:
      Set_Modus(Weiss);
      break;
    case Weiss:
      Set_Modus(Farbe);
      break;
    case Farbe:
      Set_Modus(Verlauf);
      break;
    case Verlauf:
      Set_Modus(Verlauf2);
      break;
    case Verlauf2:
      Set_Modus(Aus);
      break;
  }
  //  EEPROM.put(POS_MODUS, _Modus);
  //  EEPROM.commit();
  PlusMinus_Mode = 0;
}

void LichtModi::Prev_Modus() {
  switch (_Modus) {
    default:
    case Aus:
      Set_Modus(Verlauf2);
      break;
    case Weiss:
      Set_Modus(Aus);
      break;
    case Farbe:
      Set_Modus(Weiss);
      break;
    case Verlauf:
      Set_Modus(Farbe);
      break;
    case Verlauf2:
      Set_Modus(Verlauf);
      break;
  }
  //  EEPROM.put(POS_MODUS, _Modus);
  //  EEPROM.commit();
  PlusMinus_Mode = 0;
}

void LichtModi::Next_PlusMinus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      break;
    case Farbe:
      PlusMinus_Mode = (PlusMinus_Mode + 1) % 3;
      D_PRINTF("Neuer PM_Modus: Farbe %d\n", PlusMinus_Mode);
      break;
    case Verlauf:
      PlusMinus_Mode = (PlusMinus_Mode + 1) % 5;
      D_PRINTF("Neuer PM_Modus: Verlauf %d\n", PlusMinus_Mode);
      break;
    case Verlauf2:
      break;
  }
}

void LichtModi::Plus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      if (_Helligkeit <= 250) _Helligkeit += 5;
      else _Helligkeit = 255;
      EEPROM.put(POS_HELLIGKEIT, _Helligkeit);
      EEPROM.commit();
      break;
    case Farbe:
      D_PRINTF("+F1: x%08x, ", _Farbe1);
      uint8_t r, g, b;
      r = (_Farbe1 & 0xff0000) >> 16;
      g = (_Farbe1 & 0xff00) >> 8;
      b = (_Farbe1 & 0xff);
      switch (PlusMinus_Mode) {
        case 0:
          D_PRINTF("r von %02x ->", r);
          if (r <= 250) r += 5;
          else r = 255;
          D_PRINTF("%02x ", r);
          break;
        case 1:
          D_PRINTF("g von %02x ->", g);
          if (g <= 250) g += 5;
          else g = 255;
          D_PRINTF("%02x ", g);
          break;
        case 2:
          D_PRINTF("b von %02x ->", b);
          if (b <= 250) b += 5;
          else b = 255;
          D_PRINTF("%02x ", b);
          break;
      }
      _Farbe1 = (r << 16) + (g << 8) + b;
      float fr, fg, fb;
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      D_PRINTF(" neu: x%08x\n", _Farbe1);
      EEPROM.put(POS_FARBE1, _Farbe1);
      EEPROM.commit();
      break;
    case Verlauf:
      switch (PlusMinus_Mode) {
        case 0:
          _Speed += 10;
          if (_Speed > 1000) _Speed = 1000;
          break;
        case 1:
        case 2:
        default:
          break;
      }
      break;
    case Verlauf2:
      break;
  }
}

void LichtModi::Minus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      if (_Helligkeit > 0) {
        D_PRINTF("-H: x%02x -> ", _Helligkeit );
        if (_Helligkeit >= 5) _Helligkeit -= 5;
        else _Helligkeit = 0;
        D_PRINTF("%02x\n", _Helligkeit);
        EEPROM.put(POS_HELLIGKEIT, _Helligkeit);
        EEPROM.commit();
      }
      break;
    case Farbe:
      D_PRINTF("-F1: x%08x, ", _Farbe1);
      uint8_t r, g, b;
      r = (_Farbe1 & 0xff0000) >> 16;
      g = (_Farbe1 & 0xff00) >> 8;
      b = (_Farbe1 & 0xff);
      switch (PlusMinus_Mode) {
        case 0:
          D_PRINTF("r von %02x ->", r);
          if (r >= 5) r -= 5;
          else r = 0;
          D_PRINTF("%02x ", r);
          break;
        case 1:
          D_PRINTF("g von %02x ->", g);
          if (g >= 5) g -= 5;
          else g = 0;
          D_PRINTF("%02x ", g);
          break;
        case 2:
          D_PRINTF("b von %02x ->", b);
          if (b >= 5) b -= 5;
          else b = 0;
          D_PRINTF("%02x ", b);
          break;
      }
      _Farbe1 = (r << 16) + (g << 8) + b;
      float fr, fg, fb;
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      D_PRINTF(" neu: x%08x\n", _Farbe1);
      EEPROM.put(POS_FARBE1, _Farbe1);
      EEPROM.commit();
      break;
    case Verlauf:
      switch (PlusMinus_Mode) {
        case 0:
          _Speed -= 10;
          if (_Speed < 1000) _Speed = 0;
          break;
        case 1:
        case 2:
          break;
      }
      break;
    case Verlauf2:
      break;
  }
}

void LichtModi::Set_Helligkeit(uint8_t h, bool commit) {
  _Helligkeit = h;
  EEPROM.put(POS_HELLIGKEIT, _Helligkeit);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Set_Farbe1(uint32_t f, bool commit) {
  _Farbe1 = f;
  float fr, fg, fb;
  fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
  fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
  fb =  ((_Farbe1) & 0xff) / 255.;
  RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
  EEPROM.put(POS_FARBE1, _Farbe1);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Set_Farbe2(uint32_t f, bool commit) {
  _Farbe2 = f;
  float fr, fg, fb;
  fr =  ((_Farbe2 >> 16) & 0xff) / 255.;
  fg =  ((_Farbe2 >> 8) & 0xff) / 255.;
  fb =  ((_Farbe2) & 0xff) / 255.;
  RGBtoHSV(fr, fg, fb, _h2, _s2, _v2);
  EEPROM.put(POS_FARBE2, _Farbe2);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Set_Speed(uint16_t s, bool commit) {
  _Speed = s;
  EEPROM.put(POS_SPEED, _Speed);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Set_n_Leds(uint16_t n, bool commit) {
  _n_Leds = n;
  EEPROM.put(POS_N_LEDS, _n_Leds);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Set_Brightness(uint8_t b, bool commit) {
  _Brightness = b;
  EEPROM.put(POS_BRIGHTNESS, _Brightness);
  if (commit)
    EEPROM.commit();
}

void LichtModi::Commit() {
  EEPROM.commit();
}

LichtModi::Modi LichtModi::Get_Modus() {
  return _Modus;
}

const char* LichtModi::Get_Modus_Name() {
  switch (_Modus) {
    default:
    case Aus:
      return "Aus";
      break;
    case Weiss:
      return "Weiss";
      break;
    case Farbe:
      return "Farbe";
      break;
    case Verlauf:
      return "Verlauf";
      break;
    case Verlauf2:
      return "Verlauf2";
      break;
  }
}

uint8_t LichtModi::Get_Helligkeit() {
  return _Helligkeit;
}

uint32_t LichtModi::Get_Farbe1() {
  return _Farbe1;
}

uint32_t LichtModi::Get_Farbe2() {
  return _Farbe2;
}

uint16_t LichtModi::Get_Speed() {
  return _Speed;
}

uint16_t LichtModi::Get_n_Leds() {
  return _n_Leds;
}

uint8_t LichtModi::Get_Brightness() {
  return _Brightness;
}

void LichtModi::Tick_Verlauf() {
  float h, s, v;
  float fr, fg, fb;
  uint32_t F;

  uint8_t r, g, b;
  uint16_t n = _n_Leds; // strip.numPixels();
  float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
  for (int i = 0; i < n; i++) {
    h = f(i, t_x_v, _h1, _h2, n);
    s = f(i, t_x_v, _s1, _s2, n);
    v = f(i, t_x_v, _v1, _v2, n);
    HSVtoRGB(fr, fg, fb, h, s, v);
    r = fr * 256;
    g = fg * 256;
    b = fb * 256;
    F = (r << 16) + (g << 8) + b;
#ifdef USE_ADAFRUIT_NEOPIXEL
    strip->setPixelColor(i, F);
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
    strip->SetPixelColor(i, RgbwColor(HtmlColor(F)));
#endif // USE_NEOPIXELBUS
  }
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip->Show();
#endif // USE_NEOPIXELBUS
}

void LichtModi::Tick_Verlauf2() {
  uint16_t n = _n_Leds;
  float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
  for (int i = 0; i < n; i++) {
#ifdef USE_ADAFRUIT_NEOPIXEL
    uint16_t  h;
    uint8_t s, v;
    h = 65535 * f(i, t_x_v, _h1, _h2, n);
    s = 255 * f(i, t_x_v, _s1, _s2, n);
    v = 255 * f(i, t_x_v, _v1, _v2, n);
    strip->setPixelColor(i, strip->ColorHSV(h, s, v));
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
    float h, s, v;
    h = constrain(f(i, t_x_v, _h1, _h2, n) / 360, 0., 1.);
    s = constrain(f(i, t_x_v, _s1, _s2, n), 0., 1.);
    v = constrain(f(i, t_x_v, _v1, _v2, n), 0., 1.);
    strip->SetPixelColor(i, HsbColor(h, s, v));
#endif // USE_NEOPIXELBUS
  }
#ifdef USE_ADAFRUIT_NEOPIXEL
  strip->show();
#endif // USE_ADAFRUIT_NEOPIXEL
#ifdef USE_NEOPIXELBUS
  strip->Show();
#endif // USE_NEOPIXELBUS
}
