#ifndef _Modus_Verlauf2
#define _Modus_Verlauf2

#include "Controler.h"
#include "Color_Helper.h"

#define MODUS_VERLAUF2_F1_INIT 0xf00000
#define MODUS_VERLAUF2_F2_INIT 0x00f0000
#define MODUS_VERLAUF2_SPEED_INIT 100

class Modus_Verlauf2 : public LichtModus {
  public:
    Modus_Verlauf2(LED_Streifen *streifen): LichtModus(streifen) {
      _Farbe[0].Init(&_E_Master, MODUS_VERLAUF_F1_INIT);
      _Farbe[1].Init(&_E_Master, MODUS_VERLAUF_F2_INIT);
      _Speed.Init(&_E_Master, MODUS_VERLAUF_SPEED_INIT);
    };
    const char* Name() {
      return "Verlauf2";
    }
    const char* Params() {
      return "{\"Modus\":\"Verlauf2\",  \"Params\":  [ null ] }";
    }
    void SetParam(const char* name, const char* wert) {
      D_PRINTF("Modus Verlauf2: TODO Param: %s - %s\n", name, wert);
      return;
    }
    void Next_PlusMinus() {
    }
    void Plus() {
    }
    void Minus() {
    }
    uint8_t Status(uint8_t ab_hier, bool zeige_status) {
      if (zeige_status) {
        _streifen->SetPixelColor(ab_hier, 127, 127, 127, 0);
        return ab_hier + 1;
      } else
        return ab_hier;
    }
    void Tick(uint8_t ab_hier) {
      uint16_t n = _streifen->numPixels();
      float h[2], s[2], v[2];
      float fr, fg, fb;
      for (int i = 0; i < 2; i++) {
        fr =  (( _Farbe[n] >> 16) & 0xff) / 255.;
        fg =  (( _Farbe[n] >> 8) & 0xff) / 255.;
        fb =  (( _Farbe[n]) & 0xff) / 255.;
        RGBtoHSV(fr, fg, fb, h[n], s[n], v[n]);
      }
      float t_x_v = millis() * _Speed / 1000 * n / 256; // Zeit * Geschwindigkeit --> Weg(t)
      for (int i = ab_hier; i < n; i++) {
#if 0
        uint16_t  h;
        uint8_t s, v;
        h = 65535 * f(i, t_x_v, _h1, _h2, n);
        s = 255 * f(i, t_x_v, _s1, _s2, n);
        v = 255 * f(i, t_x_v, _v1, _v2, n);
        strip->setPixelColor(i, strip->ColorHSV(h, s, v));
#endif // 0
        uint8_t r, g, b;
        r = constrain(int(f(i, t_x_v, h[0], h[1], n) / 360 * 255), 0, 255);
        g = constrain(int(f(i, t_x_v, s[0], s[1], n) * 255), 0, 255);
        b = constrain(int(f(i, t_x_v, v[0], v[1], n) * 255), 0, 255);
        _streifen->SetPixelColor(i, r, g, b, 0);

      }
    }
    Persist<uint32_t> _Farbe[2];
    Persist<uint16_t> _Speed;
};

#endif // Modus_Verlauf2
