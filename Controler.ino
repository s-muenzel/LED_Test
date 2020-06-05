#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#include "main.h"
#include "Controler.h"

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

// "Neue" WS2812B: 800kHz (passt auf die 18.Feier Eva LEDs)
//#define LED_STRIP_TYP (NEO_GRB + NEO_KHZ800)
// WS2812: 400 kHz
//#define LED_STRIP_TYP (NEO_GRB + NEO_KHZ400)
// "Neue" SK6812(~WS2812B) RGBW (WarmWhite):
// Led Strip SK6812(Similar WS2812B) RGBW 3m 60Leds/m IP30 DC5V
#define LED_STRIP_TYP (NEO_GRBW + NEO_KHZ800)


Adafruit_NeoPixel *strip;

LichtModi::LichtModi() {
}

void LichtModi::Beginn() {
  D_PRINTLN("LichtModi Beginn");
  EEPROM.begin(GROESSE_ALLES);
  EEPROM.get(POS_MODUS, _Modus);
  EEPROM.get(POS_HELLIGKEIT, _Helligkeit);
  EEPROM.get(POS_FARBE1, _Farbe1);
  Set_Farbe1(_Farbe1);
  EEPROM.get(POS_FARBE2, _Farbe2);
  Set_Farbe2(_Farbe2);
  EEPROM.get(POS_SPEED, _Speed);
  EEPROM.get(POS_N_LEDS, _n_Leds);
  EEPROM.get(POS_BRIGHTNESS, _Brightness);
  if ((_n_Leds < 1) || (_n_Leds > 1024)) {
    D_PRINTLN("EEPROM nicht richtig initialisiert, initiere neu");
    _Modus = Aus;
    _Helligkeit = 64;
    _Farbe1 = 0x0f0f0f;
    Set_Farbe1(_Farbe1);
    _Farbe2 = 0x0f0f0f;
    Set_Farbe2(_Farbe2);
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
  strip = new Adafruit_NeoPixel(1024, LED_PIN, LED_STRIP_TYP);
  strip->begin();
  strip->show();
  delay(1);
  delete strip;
  D_PRINT("strip (0)");

  strip = new Adafruit_NeoPixel(_n_Leds, LED_PIN, LED_STRIP_TYP);
  strip->begin();
  strip->show();
  strip->setBrightness(_Brightness);
  D_PRINTLN("ENDE LichtModi");
}

void LichtModi::Bereit() {
}

void LichtModi::Tick() {
  switch (_Modus) {
    default:
    case Aus:
      strip->fill();
      strip->show();
      break;
    case Weiss:
      // strip->fill(strip->Color(strip->gamma8(_Helligkeit), strip->gamma8(_Helligkeit), strip->gamma8(_Helligkeit)));
      strip->fill(strip->Color(0, 0, 0, strip->gamma8(_Helligkeit)));
      strip->show();
      break;
    case Farbe:
      strip->fill(_Farbe1);
      strip->show();
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
  EEPROM.put(POS_MODUS, _Modus);
  if (commit)
    EEPROM.commit();
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
  EEPROM.put(POS_MODUS, _Modus);
  if (commit)
    EEPROM.commit();
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
    strip->setPixelColor(i, F);
  }
  strip->show();
}

void LichtModi::Tick_Verlauf2() {
  uint16_t  h;
  uint8_t s, v;
  uint16_t n = _n_Leds; // strip.numPixels();
  float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
  for (int i = 0; i < n; i++) {
    h = 65535*f(i, t_x_v, _h1, _h2, n);
    s = 255*f(i, t_x_v, _s1, _s2, n);
    v = 255*f(i, t_x_v, _v1, _v2, n);
    strip->setPixelColor(i, strip->ColorHSV(h,s,v));
  }
  strip->show();
}

/*! \brief Convert RGB to HSV color space

  Converts a given set of RGB values `r', `g', `b' into HSV
  coordinates. The input RGB values are in the range [0, 1], and the
  output HSV values are in the ranges h = [0, 360], and s, v = [0,
  1], respectively.

  \param fR Red component, used as input, range: [0, 1]
  \param fG Green component, used as input, range: [0, 1]
  \param fB Blue component, used as input, range: [0, 1]
  \param fH Hue component, used as output, range: [0, 360]
  \param fS Hue component, used as output, range: [0, 1]
  \param fV Hue component, used as output, range: [0, 1]

*/
void RGBtoHSV(float& fR, float& fG, float fB, float& fH, float& fS, float& fV) {
  float fCMax = max(max(fR, fG), fB);
  float fCMin = min(min(fR, fG), fB);
  float fDelta = fCMax - fCMin;

  if (fDelta > 0) {
    if (fCMax == fR) {
      fH = 60 * (fmod(((fG - fB) / fDelta), 6));
    } else if (fCMax == fG) {
      fH = 60 * (((fB - fR) / fDelta) + 2);
    } else if (fCMax == fB) {
      fH = 60 * (((fR - fG) / fDelta) + 4);
    }

    if (fCMax > 0) {
      fS = fDelta / fCMax;
    } else {
      fS = 0;
    }

    fV = fCMax;
  } else {
    fH = 0;
    fS = 0;
    fV = fCMax;
  }

  if (fH < 0) {
    fH = 360 + fH;
  }
}


/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param fR Red component, used as output, range: [0, 1]
  \param fG Green component, used as output, range: [0, 1]
  \param fB Blue component, used as output, range: [0, 1]
  \param fH Hue component, used as input, range: [0, 360]
  \param fS Hue component, used as input, range: [0, 1]
  \param fV Hue component, used as input, range: [0, 1]

*/
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
  float fC = fV * fS; // Chroma
  float fHPrime = fmod(fH / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fV - fC;

  if (0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if (1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if (2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if (3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if (4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if (5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }

  fR += fM;
  fG += fM;
  fB += fM;
}

float g(int x, float t_x_v, uint16_t n) {
  return ((int)(t_x_v - x)) % (n);
}

float f(int x, float t_x_v, float a, float b, uint16_t n) {
  if (g(x, t_x_v, n) < n / 2) {
    return a + (b - a) * g(x, t_x_v, n) / (n / 2);
  } else {
    return b + (a - b) * g(x - (n / 2), t_x_v, n) / (n / 2);
  }
}
