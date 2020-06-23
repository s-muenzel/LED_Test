#ifndef _LichtModi
#define _LichtModi

#include "Persistenz.h"

#include "LED_Streifen.h"


EEPROM_Master _E_Master;

class LichtModus {
  public:
    LichtModus(LED_Streifen *streifen) {
      _streifen = streifen;
    };
    virtual const char* Name();
    virtual const char* Params();
    virtual void SetParam(const char* name, const char* wert);
    virtual void Next_PlusMinus();
    virtual void Plus();
    virtual void Minus();

    virtual uint8_t Status(uint8_t ab_hier, bool zeige_status);
    virtual void Tick(uint8_t ab_hier);
  protected:
    LED_Streifen *_streifen;

    void Indikator(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w, bool an, bool state) {
      if (!state) {
        _streifen->SetPixelColor(n, r, g, b, w);
      } else {
        if (an) {
          if ( (r != 0) || (g != 0) || (b != 0) || (w != 0)) {
            _streifen->SetPixelColor(n, r, g, b, w);
          } else {
            _streifen->SetPixelColor(n, 10, 10, 10, 0);
          }
        } else {
          _streifen->SetPixelColor(n, 0, 0, 0, 0);
        }
      }
    }
};


#include "Modus_Aus.h"
#include "Modus_Weiss.h"
#include "Modus_Farbe.h"
#include "Modus_Verlauf.h"
#include "Modus_Verlauf2.h"

class LichtModi {
  public:
    LichtModi();

    void Beginn();
    void Bereit();

    void Tick();

    typedef enum { Aus = 0, Weiss = 1, Farbe = 2, Verlauf = 3, Verlauf2 = 4, letzter_Modus} Modi;

    void Set_Modus(Modi mode);
    void Set_Modus(const char*mode);
    void Next_Modus();
    void Prev_Modus();
    Modi Get_Modus() {
      return _Modus;
    };
    const char* Get_Modus_Name() {
      return _Modi[_Modus]->Name();
    };
    uint8_t Get_Count_of_Modi() {
      return (uint8_t)((Modi)letzter_Modus);
    }
    const char* Get_Modus_Name(uint8_t n) {
      return _Modi[n]->Name();
    }
    const char* Get_Modus_Params() {
      return _Modi[_Modus]->Params();
    }
    void Set_Modus_Param(const char* name, const char* wert) {
      _Modi[_Modus]->SetParam(name, wert);
    }
    const char* Get_Modus_Params(const char*mode);
    void Next_PlusMinus();
    void Plus();
    void Minus();

    void Set_n_Leds(uint16_t n);
    uint16_t Get_n_Leds();
    void Commit();

  protected:

    LED_Streifen _Streifen;

    Modi _Modus;
    std::vector<LichtModus*> _Modi;

    // Persistent Data

    Persist<uint16_t> _Version;
    Persist<uint16_t> _n_Leds;

    // Transient Data
    uint32_t Status_Timer;
    uint32_t last_led_update;

};

#endif // _LichtModi
