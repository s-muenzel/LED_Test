#ifndef _Modus_Farbe
#define _Modus_Farbe

#include "Controler.h"

#define MODUS_FARBE_INIT 0xf00000

class Modus_Farbe : public LichtModus {
  public:
    Modus_Farbe(LED_Streifen *streifen): LichtModus(streifen) {
      opts = rot;
      _Farbe.Init(&_E_Master, MODUS_FARBE_INIT);
    };
    const char* Name() {
      return "Farbe";
    }
    const char* Params() {
      static char txt[160];
      sprintf(txt, "{\"Modus\":\"Farbe\",\"Params\":[{\"Name\":\"Farbe\",\"Typ\":\"color\",\"Wert\":\"#%06x\"},{\"Name\":\"Weiss\",\"Typ\":\"range\",\"Wert\":\"%d\",\"Min\":\"0\",\"Max\":\"255\"}]}", 0xffffff & (uint32_t)_Farbe, (uint32_t)_Farbe >> 24);
      return txt;
    }
    void SetParam(const char* name, const char* wert) {
      if (strcmp(name, "Farbe") == 0) {
        D_PRINTF("Neue Farbe (Farbwert-string):%s", wert);
        uint32_t f = strtol(wert + 1, 0, 16); // Farben haben ein "#" am Anfang
        D_PRINTF("Neue Farbe (Farbwert-int):0x%08x\n", f);
        _Farbe = ((uint32_t)_Farbe & 0xff000000) + f;
        D_PRINTF("Neue Farbe: 0x%08x\n", (uint32_t)_Farbe);
      } else if (strcmp(name, "Weiss") == 0) {
        D_PRINTF("Neue Farbe (Weisswert):%s", wert);
        uint32_t h = atoi(wert);
        _Farbe = (0x00ffffff & (uint32_t)_Farbe) | (h << 24);
        D_PRINTF("Neue Farbe: 0x%08x\n", (uint32_t)_Farbe);
      } else {
        D_PRINTF("Farbe: Fehler bei Params: %s - %s\n", name, wert);
      }
      return;
    }
    void Next_PlusMinus() {
      opts = (Optionen)((opts + 1) % opt_max);
    }
    void Plus() {
      uint8_t r, g, b, w;
      r = (_Farbe & 0xff0000) >> 16;
      g = (_Farbe & 0xff00) >> 8;
      b = (_Farbe & 0xff);
      w = (_Farbe & 0xff000000) >> 24;
      switch (opts) {
        case rot:
          if (r == 255)
            return;
          D_PRINTF("Farbe+rot   x%08x", (uint32_t)_Farbe);
          if (r <= 250) r += 5;
          else r = 255;
          break;
        case gruen:
          if (g == 255)
            return;
          D_PRINTF("Farbe+gruen x%08x", (uint32_t)_Farbe);
          if (g <= 250) g += 5;
          else g = 255;
          break;
        case blau:
          if (b == 255)
            return;
          D_PRINTF("Farbe+blau  x%08x", (uint32_t)_Farbe);
          if (b <= 250) b += 5;
          else b = 255;
          break;
        case weiss:
          if (w == 255)
            return;
          D_PRINTF("Farbe+weiss x%08x", (uint32_t)_Farbe);
          if (w <= 250) w += 5;
          else w = 255;
          break;
      }
      _Farbe = (r << 16) + (g << 8) + b + (w << 24);
      D_PRINTF(" neu x%08x\n", (uint32_t)_Farbe);
    }
    void Minus() {
      uint8_t r, g, b, w;
      r = (_Farbe & 0xff0000) >> 16;
      g = (_Farbe & 0xff00) >> 8;
      b = (_Farbe & 0xff);
      w = (_Farbe & 0xff000000) >> 24;
      switch (opts) {
        case rot:
          if (r == 0)
            return;
          D_PRINTF("Farbe-rot   x%08x", (uint32_t)_Farbe);
          if (r >= 5) r -= 5;
          else r = 0;
          break;
        case gruen:
          if (g == 0)
            return;
          D_PRINTF("Farbe-gruen x%08x", (uint32_t)_Farbe);
          if (g >= 5) g -= 5;
          else g = 0;
          break;
        case blau:
          if (b == 0)
            return;
          D_PRINTF("Farbe-blau  x%08x", (uint32_t)_Farbe);
          if (b >= 5) b -= 5;
          else b = 0;
          break;
        case weiss:
          if (w == 0)
            return;
          D_PRINTF("Farbe-weiss x%08x", (uint32_t)_Farbe);
          if (w >= 5) w -= 5;
          else w = 0;
          break;
      }
      _Farbe = (r << 16) + (g << 8) + b + (w << 24);
      D_PRINTF(" neu x%08x\n", (uint32_t)_Farbe);
    }
    uint8_t Status(uint8_t ab_hier, bool zeige_status) {
      if (zeige_status) {
        bool blink_aus = (millis() % 1000) > 500;
        _streifen->SetPixelColor(ab_hier, 127, 0, 0, 0);
        Indikator(ab_hier + 1, (_Farbe >> 16) & 0xff, 0, 0, 0, blink_aus, (opts == rot));
        Indikator(ab_hier + 2, 0, (_Farbe >>  8) & 0xff, 0, 0, blink_aus, (opts == gruen));
        Indikator(ab_hier + 3, 0, 0, (_Farbe >>  0) & 0xff, 0, blink_aus, (opts == blau));
        Indikator(ab_hier + 4, 0, 0, 0, (_Farbe >> 24) & 0xff, blink_aus, (opts == weiss));
        return ab_hier + 5;
      } else
        return ab_hier;
    }
    void Tick(uint8_t ab_hier) {
      _streifen->MonoFarbe((_Farbe >> 16) & 0xff, (_Farbe >> 8) & 0xff, (_Farbe >> 0) & 0xff, (_Farbe >> 24) & 0xff, ab_hier);
    }
  protected:
    typedef enum { rot = 0, gruen = 1, blau = 2, weiss = 3, opt_max} Optionen;
    Optionen opts;
    Persist<uint32_t> _Farbe;
};

#endif // Modus_Farbe
