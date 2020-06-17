#include <EEPROM.h>
#include "main.h"
#include "Controler.h"
#include "Color_Helper.h"

#define STATUS_DAUER  5000 // 5 sekunden Status anzeigen

#define E_VERSION         3.0
#define E_HELLIGKEIT    128
#define E_FARBE1        0xf00000
#define E_FARBE2        0x00f000
#define E_SPEED         100
#define E_N_LEDS        150
#define E_BRIGHTNESS    64

#include "LED_Streifen.h"
LED_Streifen _Streifen;

LichtModi::LichtModi() {
  PlusMinus_Mode = 0;
  Status_Timer = 0;
}

void LichtModi::Beginn() {
  D_PRINTLN("LichtModi Beginn");
  _Version.Init(&_E_Master, E_VERSION);
  _Helligkeit.Init(&_E_Master, E_HELLIGKEIT);
  _Farbe1.Init(&_E_Master, E_FARBE1);
  _Farbe2.Init(&_E_Master, E_FARBE2);
  _Speed.Init(&_E_Master, E_SPEED);
  _n_Leds.Init(&_E_Master, E_N_LEDS);
  _Brightness.Init(&_E_Master, E_BRIGHTNESS);

  _E_Master.Beginn();
  if (_Version != E_VERSION)
    _E_Master.ResetAll();

  _Modus = Weiss;
  Set_Farbe1(_Farbe1);
  Set_Farbe2(_Farbe2);

  _Streifen.Beginn(_n_Leds, _Brightness);
  D_PRINTLN("ENDE LichtModi");
}

void LichtModi::Bereit() {
  _Streifen.Bereit();
}

void LichtModi::Tick() {
  _E_Master.Tick();
  uint8_t n_status_pixel;
  switch (_Modus) {
    default:
    case Aus:
      n_status_pixel = Status_Aus();
      Tick_Aus(n_status_pixel);
      break;
    case Weiss:
      n_status_pixel = Status_Weiss();
      Tick_Weiss(n_status_pixel);
      break;
    case Farbe:
      n_status_pixel = Status_Farbe();
      Tick_Farbe(n_status_pixel);
      break;
    case Verlauf:
      n_status_pixel = Status_Verlauf();
      Tick_Verlauf(n_status_pixel);
      break;
    case Verlauf2:
      n_status_pixel = Status_Verlauf2();
      Tick_Verlauf2(n_status_pixel);
      break;
  }
  _Streifen.Show();
}

void LichtModi::Set_Modus(Modi mode, bool commit) {
  _Modus = mode;
  D_PRINTF("Neuer Modus: %s\n", Get_Modus_Name());
  PlusMinus_Mode = 0;
  Status_Timer = millis() + STATUS_DAUER;
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
  PlusMinus_Mode = 0;
  Status_Timer = millis() + STATUS_DAUER;
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
  PlusMinus_Mode = 0;
  Status_Timer = millis() + STATUS_DAUER;
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
  PlusMinus_Mode = 0;
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Next_PlusMinus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      break;
    case Farbe:
      PlusMinus_Mode = (PlusMinus_Mode + 1) % 4;
      D_PRINTF("Neuer PM_Modus: Farbe %d\n", PlusMinus_Mode);
      break;
    case Verlauf:
      PlusMinus_Mode = (PlusMinus_Mode + 1) % 7;
      D_PRINTF("Neuer PM_Modus: Verlauf %d\n", PlusMinus_Mode);
      break;
    case Verlauf2:
      break;
  }
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Plus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      if (_Helligkeit <= 250) _Helligkeit = _Helligkeit + 5;
      else _Helligkeit = 255;
      break;
    case Farbe:
      D_PRINTF("+F1: x%08x, ", _Farbe1);
      uint8_t r, g, b, w;
      r = (_Farbe1 & 0xff0000) >> 16;
      g = (_Farbe1 & 0xff00) >> 8;
      b = (_Farbe1 & 0xff);
      w = (_Farbe1 & 0xff000000) >> 24;
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
        case 3:
          D_PRINTF("w von %02x ->", w);
          if (w <= 250) w += 5;
          else w = 255;
          D_PRINTF("%02x ", w);
          break;
      }
      _Farbe1 = (r << 16) + (g << 8) + b + (w << 24);
      float fr, fg, fb;
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      D_PRINTF(" neu: x%08x\n", _Farbe1);
      break;
    case Verlauf:
      D_PRINTF("+F1: x%08x, F2: x%08x ", _Farbe1, _Farbe2);
      uint8_t r1, g1, b1, w1;
      r1 = (_Farbe1 & 0xff0000) >> 16;
      g1 = (_Farbe1 & 0xff00) >> 8;
      b1 = (_Farbe1 & 0xff);
      w1 = (_Farbe1 & 0xff000000) >> 24;
      uint8_t r2, g2, b2, w2;
      r2 = (_Farbe2 & 0xff0000) >> 16;
      g2 = (_Farbe2 & 0xff00) >> 8;
      b2 = (_Farbe2 & 0xff);
      w2 = (_Farbe2 & 0xff000000) >> 24;
      switch (PlusMinus_Mode) {
        case 0:
          if (_Speed > 990) _Speed = 1000;
          else _Speed = _Speed + 10;
          break;
        case 1:
          D_PRINTF("r1 von %02x ->", r1);
          if (r1 <= 250) r1 += 5;
          else r1 = 255;
          D_PRINTF("%02x ", r1);
          break;
        case 2:
          D_PRINTF("g1 von %02x ->", g1);
          if (g1 <= 250) g1 += 5;
          else g1 = 255;
          D_PRINTF("%02x ", g1);
          break;
        case 3:
          D_PRINTF("b1 von %02x ->", b1);
          if (b1 <= 250) b1 += 5;
          else b1 = 255;
          D_PRINTF("%02x ", b1);
          break;
        case 4:
          D_PRINTF("r2 von %02x ->", r2);
          if (r2 <= 250) r2 += 5;
          else r2 = 255;
          D_PRINTF("%02x ", r2);
          break;
        case 5:
          D_PRINTF("g2 von %02x ->", g2);
          if (g2 <= 250) g2 += 5;
          else g2 = 255;
          D_PRINTF("%02x ", g2);
          break;
        case 6:
          D_PRINTF("b2 von %02x ->", b2);
          if (b2 <= 250) b2 += 5;
          else b2 = 255;
          D_PRINTF("%02x ", b2);
          break;
        default:
          break;
      }
      _Farbe1 = (r1 << 16) + (g1 << 8) + b1 + (w1 << 24);
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      _Farbe2 = (r2 << 16) + (g2 << 8) + b2 + (w2 << 24);
      fr =  ((_Farbe2 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe2 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe2) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h2, _s2, _v2);
      D_PRINTF(" neu: x%08x, x%08x\n", _Farbe1, _Farbe2);
      break;
    case Verlauf2:
      break;
  }
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Minus() {
  switch (_Modus) {
    default:
    case Aus:
      break;
    case Weiss:
      if (_Helligkeit > 0) {
        D_PRINTF("-H: x%02x -> ", _Helligkeit );
        if (_Helligkeit >= 5) _Helligkeit = _Helligkeit - 5;
        else _Helligkeit = 0;
        D_PRINTF("%02x\n", _Helligkeit);
      }
      break;
    case Farbe:
      D_PRINTF("-F1: x%08x, ", _Farbe1);
      uint8_t r, g, b, w;
      r = (_Farbe1 & 0xff0000) >> 16;
      g = (_Farbe1 & 0xff00) >> 8;
      b = (_Farbe1 & 0xff);
      w = (_Farbe1 & 0xff000000) << 24;
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
        case 3:
          D_PRINTF("w von %02x ->", w);
          if (w >= 5) w -= 5;
          else w = 0;
          D_PRINTF("%02x ", w);
          break;
      }
      _Farbe1 = (r << 16) + (g << 8) + b + (w << 24);
      float fr, fg, fb;
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      D_PRINTF(" neu: x%08x\n", _Farbe1);
      break;
    case Verlauf:
      D_PRINTF("+F1: x%08x, F2: x%08x ", _Farbe1, _Farbe2);
      uint8_t r1, g1, b1, w1;
      r1 = (_Farbe1 & 0xff0000) >> 16;
      g1 = (_Farbe1 & 0xff00) >> 8;
      b1 = (_Farbe1 & 0xff);
      w1 = (_Farbe1 & 0xff000000) >> 24;
      uint8_t r2, g2, b2, w2;
      r2 = (_Farbe2 & 0xff0000) >> 16;
      g2 = (_Farbe2 & 0xff00) >> 8;
      b2 = (_Farbe2 & 0xff);
      w2 = (_Farbe2 & 0xff000000) >> 24;
      switch (PlusMinus_Mode) {
        case 0:
          if (_Speed < 10) _Speed = 0;
          else _Speed = _Speed - 10;
          break;
        case 1:
          D_PRINTF("r1 von %02x ->", r1);
          if (r1 >= 5) r1 -= 5;
          else r1 = 0;
          D_PRINTF("%02x ", r1);
          break;
        case 2:
          D_PRINTF("g1 von %02x ->", g1);
          if (g1 >= 5) g1 -= 5;
          else g1 = 0;
          D_PRINTF("%02x ", g1);
          break;
        case 3:
          D_PRINTF("b1 von %02x ->", b1);
          if (b1 >= 5) b1 -= 5;
          else b1 = 0;
          D_PRINTF("%02x ", b1);
          break;
        case 4:
          D_PRINTF("r2 von %02x ->", r2);
          if (r2 >= 5) r2 -= 5;
          else r2 = 0;
          D_PRINTF("%02x ", r2);
          break;
        case 5:
          D_PRINTF("g2 von %02x ->", g2);
          if (g2 >= 5) g2 -= 5;
          else g2 = 0;
          D_PRINTF("%02x ", g2);
          break;
        case 6:
          D_PRINTF("b2 von %02x ->", b2);
          if (b2 >= 5) b2 -= 5;
          else b2 = 0;
          D_PRINTF("%02x ", b2);
          break;
        default:
          break;
      }
      _Farbe1 = (r1 << 16) + (g1 << 8) + b1 + (w1 << 24);
      fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe1) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
      _Farbe2 = (r2 << 16) + (g2 << 8) + b2 + (w2 << 24);
      fr =  ((_Farbe2 >> 16) & 0xff) / 255.;
      fg =  ((_Farbe2 >> 8) & 0xff) / 255.;
      fb =  ((_Farbe2) & 0xff) / 255.;
      RGBtoHSV(fr, fg, fb, _h2, _s2, _v2);
      D_PRINTF(" neu: x%08x, x%08x\n", _Farbe1, _Farbe2);
      break;
    case Verlauf2:
      break;
  }
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_Helligkeit(uint8_t h, bool commit) {
  _Helligkeit = h;
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_Farbe1(uint32_t f, bool commit) {
  _Farbe1 = f;
  float fr, fg, fb;
  fr =  ((_Farbe1 >> 16) & 0xff) / 255.;
  fg =  ((_Farbe1 >> 8) & 0xff) / 255.;
  fb =  ((_Farbe1) & 0xff) / 255.;
  RGBtoHSV(fr, fg, fb, _h1, _s1, _v1);
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_Farbe2(uint32_t f, bool commit) {
  _Farbe2 = f;
  float fr, fg, fb;
  fr =  ((_Farbe2 >> 16) & 0xff) / 255.;
  fg =  ((_Farbe2 >> 8) & 0xff) / 255.;
  fb =  ((_Farbe2) & 0xff) / 255.;
  RGBtoHSV(fr, fg, fb, _h2, _s2, _v2);
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_Speed(uint16_t s, bool commit) {
  _Speed = s;
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_n_Leds(uint16_t n, bool commit) {
  _n_Leds = n;
}

void LichtModi::Set_Brightness(uint8_t b, bool commit) {
  _Brightness = b;
}

void LichtModi::Commit() {
  _E_Master.Tick(true);
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

void LichtModi::Indikator(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w, bool an, bool state) {
  if (!state) {
    _Streifen.SetPixelColor(n, r, g, b, w);
  } else {
    if (an) {
      if ( (r != 0) || (g != 0) || (b != 0)) {
        _Streifen.SetPixelColor(n, r, g, b, w);
      } else {
        _Streifen.SetPixelColor(n, 10, 10, 10, 0);
      }
    } else {
      _Streifen.SetPixelColor(n, 0, 0, 0, 0);
    }
  }
}

uint8_t LichtModi::Status_Aus() {
  return 0;
}

void LichtModi::Tick_Aus(uint8_t ab_hier) {
  _Streifen.MonoFarbe(0, 0, 0, 0, ab_hier);
}

uint8_t LichtModi::Status_Weiss() {
  if (millis() < Status_Timer) {
    _Streifen.SetPixelColor(0, 0, 0, 0, 127);
    return 1;
  } else
    return 0;
}

void LichtModi::Tick_Weiss(uint8_t ab_hier) {
  _Streifen.MonoFarbe(0, 0, 0, _Helligkeit, ab_hier);
}

uint8_t LichtModi::Status_Farbe() {
  if (millis() < Status_Timer) {
    bool blink_aus = (millis() % 1000) > 500;
    _Streifen.SetPixelColor(0, 127, 0, 0, 0);
    Indikator(1, (_Farbe1 >> 16) & 0xff, 0, 0, 0, blink_aus, (PlusMinus_Mode == 0));
    Indikator(2, 0, (_Farbe1 >>  8) & 0xff, 0, 0, blink_aus, (PlusMinus_Mode == 1));
    Indikator(3, 0, 0, (_Farbe1 >>  0) & 0xff, 0, blink_aus, (PlusMinus_Mode == 2));
    Indikator(4, 0, 0, 0, (_Farbe1 >> 24) & 0xff, blink_aus, (PlusMinus_Mode == 3));
    return 5;
  } else
    return 0;
}

void LichtModi::Tick_Farbe(uint8_t ab_hier) {
  _Streifen.MonoFarbe((_Farbe1 >> 16) & 0xff, (_Farbe1 >> 8) & 0xff, (_Farbe1 >> 0) & 0xff, (_Farbe1 >> 24) & 0xff, ab_hier);
}

uint8_t LichtModi::Status_Verlauf() {
  if (millis() < Status_Timer) {
    bool blink_aus;
    if (_Speed > 2)
      blink_aus = ( millis() % (10 * _Speed)) > (_Speed * 5);
    else
      blink_aus = false;
    Indikator(4, 32, 32, 32, 0, blink_aus, (PlusMinus_Mode == 0));
    blink_aus = (millis() % 1000) > 500;
    _Streifen.SetPixelColor(0, 127, 0, 127, 0);
    Indikator(1, (_Farbe1 >> 16) & 0xff, 0, 0, 0, blink_aus, (PlusMinus_Mode == 1));
    Indikator(2, 0, (_Farbe1 >>  8) & 0xff, 0, 0, blink_aus, (PlusMinus_Mode == 2));
    Indikator(3, 0, 0, (_Farbe1 >>  0) & 0xff, 0, blink_aus, (PlusMinus_Mode == 3));
    Indikator(5, (_Farbe2 >> 16) & 0xff, 0, 0, 0, blink_aus, (PlusMinus_Mode == 4));
    Indikator(6, 0, (_Farbe2 >>  8) & 0xff, 0, 0, blink_aus, (PlusMinus_Mode == 5));
    Indikator(7, 0, 0, (_Farbe2 >>  0) & 0xff, 0, blink_aus, (PlusMinus_Mode == 6));
    return 8;
  } else
    return 0;
}

void LichtModi::Tick_Verlauf(uint8_t ab_hier) {
  float h, s, v;
  float fr, fg, fb;
  uint32_t F;

  uint8_t r, g, b;
  uint16_t n = _n_Leds; // strip.numPixels();
  float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
  for (int i = ab_hier; i < n; i++) {
    h = f(i, t_x_v, _h1, _h2, n);
    s = f(i, t_x_v, _s1, _s2, n);
    v = f(i, t_x_v, _v1, _v2, n);
    HSVtoRGB(fr, fg, fb, h, s, v);
    r = fr * 256;
    g = fg * 256;
    b = fb * 256;
    F = (r << 16) + (g << 8) + b;
    _Streifen.SetPixelColor(i, r, g, b, 0);
  }
}

uint8_t LichtModi::Status_Verlauf2() {
  if (millis() < Status_Timer) {
    _Streifen.SetPixelColor(0, 127, 127, 127, 0);
    return 1;
  } else
    return 0;
}

void LichtModi::Tick_Verlauf2(uint8_t ab_hier) {
  uint16_t n = _n_Leds;
  float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
  for (int i = ab_hier; i < n; i++) {
#if 0
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
#endif // 0
    uint8_t r, g, b;
    r = constrain(int(f(i, t_x_v, _h1, _h2, n) / 360 * 255), 0, 255);
    g = constrain(int(f(i, t_x_v, _s1, _s2, n) * 255), 0, 255);
    b = constrain(int(f(i, t_x_v, _v1, _v2, n) * 255), 0, 255);
    _Streifen.SetPixelColor(i, r, g, b, 0);

  }
}
