#ifndef _Modus_Verlauf
#define _Modus_Verlauf

#include "Controler.h"
#include "Color_Helper.h"


#define MODUS_VERLAUF_F1_INIT 0xf00000
#define MODUS_VERLAUF_F2_INIT 0x00f0000
#define MODUS_VERLAUF_SPEED_INIT 100


class Modus_Verlauf : public LichtModus {
  public:
    Modus_Verlauf(LED_Streifen *streifen): LichtModus(streifen) {
      opts = speed;
      _Farbe[0].Init(&_E_Master, MODUS_VERLAUF_F1_INIT);
      _Farbe[1].Init(&_E_Master, MODUS_VERLAUF_F2_INIT);
      _Speed.Init(&_E_Master, MODUS_VERLAUF_SPEED_INIT);
    };
    const char* Name() {
      return "Verlauf";
    }
    void Next_PlusMinus() {
      opts = (Optionen)((opts + 1) % opt_max);
    }
    void Plus() {
      uint8_t n = (opts >= rot2) ? 1 : 0;
      uint8_t r, g, b, w;
      r = (_Farbe[n] & 0xff0000) >> 16;
      g = (_Farbe[n] & 0xff00) >> 8;
      b = (_Farbe[n] & 0xff);
      switch (opts) {
        case speed:
          if (_Speed == 1000)
            return;
          D_PRINTF("speed von %d ->", (uint16_t)_Speed);
          if (_Speed > 990) _Speed = 1000;
          else _Speed = _Speed + 10;
          D_PRINTF("%d ", (uint16_t)_Speed);
          return;
          break;
        case rot1:
        case rot2:
          if (r == 255)
            return;
          D_PRINTF("Farbe+rot   x%08x", _Farbe[n]);
          if (r <= 250) r += 5;
          else r = 255;
          break;
        case gruen1:
        case gruen2:
          if (g == 255)
            return;
          D_PRINTF("Farbe+gruen x%08x", _Farbe[n]);
          if (g <= 250) g += 5;
          else g = 255;
          break;
        case blau1:
        case blau2:
          if (b == 255)
            return;
          D_PRINTF("Farbe+blau  x%08x", _Farbe[n]);
          if (b <= 250) b += 5;
          else b = 255;
          break;
      }
      _Farbe[n] = (r << 16) + (g << 8) + b;
      D_PRINTF(" neu x%08x\n", _Farbe[n]);
    }
    void Minus() {
      uint8_t n = (opts >= rot2) ? 1 : 0;
      uint8_t r, g, b, w;
      r = (_Farbe[n] & 0xff0000) >> 16;
      g = (_Farbe[n] & 0xff00) >> 8;
      b = (_Farbe[n] & 0xff);
      switch (opts) {
        case speed:
          if (_Speed == 0)
            return;
          D_PRINTF("speed von %d ->", (uint16_t)_Speed);
          if (_Speed < 10) _Speed = 0;
          else _Speed = _Speed - 10;
          D_PRINTF("%d ", (uint16_t)_Speed);
          return;
        case rot1:
        case rot2:
          if (r == 0)
            return;
          D_PRINTF("Farbe-rot   x%08x", _Farbe[n]);
          if (r >= 5) r -= 5;
          else r = 0;
          break;
        case gruen1:
        case gruen2:
          if (g == 0)
            return;
          D_PRINTF("Farbe-gruen x%08x", _Farbe[n]);
          if (g >= 5) g -= 5;
          else g = 0;
          break;
        case blau1:
        case blau2:
          if (b == 0)
            return;
          D_PRINTF("Farbe-blau  x%08x", _Farbe[n]);
          if (b >= 5) b -= 5;
          else b = 0;
          break;
      }
      _Farbe[n] = (r << 16) + (g << 8) + b;
      D_PRINTF(" neu x%08x\n", _Farbe[n]);
    }
    uint8_t Status(uint8_t ab_hier, bool zeige_status) {
      if (zeige_status) {
        bool blink_aus;
        _streifen->SetPixelColor(ab_hier, 127, 0, 127, 0);
        // Range: 5Hz -> 0.1Hz
        // millis()*Speed / 5000ms*1000 / 100000 / --> 10
        //                  5000ms*10 /    50000 --> 0.2
        if (_Speed < 1000)
          blink_aus = (millis() % (10 * (1000 - _Speed))) > (5 * (1000 - _Speed));
        else
          blink_aus = true;
        Indikator(ab_hier + 1, 32, 32, 32, 0, blink_aus, (opts == speed));
        blink_aus = (millis() % 1000) > 500;
        Indikator(ab_hier + 2, (_Farbe[0] >> 16) & 0xff, 0, 0, 0, blink_aus, (opts == rot1));
        Indikator(ab_hier + 3, 0, (_Farbe[0] >>  8) & 0xff, 0, 0, blink_aus, (opts == gruen1));
        Indikator(ab_hier + 4, 0, 0, (_Farbe[0] >>  0) & 0xff, 0, blink_aus, (opts == blau1));
        Indikator(ab_hier + 5, (_Farbe[1] >> 16) & 0xff, 0, 0, 0, blink_aus, (opts == rot2));
        Indikator(ab_hier + 6, 0, (_Farbe[1] >>  8) & 0xff, 0, 0, blink_aus, (opts == gruen2));
        Indikator(ab_hier + 7, 0, 0, (_Farbe[1] >>  0) & 0xff, 0, blink_aus, (opts == blau2));
        return ab_hier + 8;
      } else
        return ab_hier;
    }
    void Tick(uint8_t ab_hier) {
      uint16_t n = _streifen->numPixels();
      float h[2], s[2], v[2];
      float h1, s1, v1;
      float fr, fg, fb;
      uint8_t r, g, b;
      uint32_t F;
      for (int i = 0; i < 2; i++) {
        fr =  (( _Farbe[i] >> 16) & 0xff) / 255.;
        fg =  (( _Farbe[i] >> 8) & 0xff) / 255.;
        fb =  (( _Farbe[i]) & 0xff) / 255.;
        RGBtoHSV(fr, fg, fb, h[i], s[i], v[i]);
      }
      float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
      for (int i = ab_hier; i < n; i++) {
        h1 = f(i, t_x_v, h[0], h[1], n);
        s1 = f(i, t_x_v, s[0], s[1], n);
        v1 = f(i, t_x_v, v[0], v[1], n);
        HSVtoRGB(fr, fg, fb, h1, s1, v1);
        r = fr * 256;
        g = fg * 256;
        b = fb * 256;
        F = (r << 16) + (g << 8) + b;
        _streifen->SetPixelColor(i, r, g, b, 0);
      }
    }
  protected:
    typedef enum { speed = 0, rot1 = 1, gruen1 = 2, blau1 = 3, rot2 = 4, gruen2 = 5, blau2 = 6,  opt_max} Optionen;
    Optionen opts;
    Persist<uint32_t> _Farbe[2];
    Persist<uint16_t> _Speed;
};

#endif // Modus_Verlauf
