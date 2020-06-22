#ifndef _Modus_Weiss
#define _Modus_Weiss

#include "Controler.h"

#define MODUS_WEISS_H_INIT 128
#define MODUS_WEISS_SH_INIT 0

class Modus_Weiss : public LichtModus {
  public:
    Modus_Weiss(LED_Streifen *streifen): LichtModus(streifen) {
      _Helligkeit.Init(&_E_Master, MODUS_WEISS_H_INIT);
      _SuperHell.Init(&_E_Master, MODUS_WEISS_SH_INIT);
    };
    const char* Name() {
      return "Weiss";
    }
    void Next_PlusMinus() {
    }
    void Plus() {
      if (_Helligkeit == 255) {
        if (_SuperHell == 255) {
          return; // Already @max
        } else {
          if (_SuperHell <= (255 - 10)) {
            _SuperHell = _SuperHell + 10;
          } else {
            _SuperHell = 255;
          }
        }
      } else {
        if (_Helligkeit <= (255 - 5)) {
          _Helligkeit = _Helligkeit + 5;
        } else {
          _Helligkeit = 255;
        }
      }
      D_PRINTF("Neue Helligkeit(+): Norm %d( Super %d)\n",(uint8_t)_Helligkeit, (uint8_t)_SuperHell);
    }
    void Minus() {
      if (_SuperHell > 0) {
        if (_SuperHell > 10) {
          _SuperHell = _SuperHell - 10;
        } else {
          _SuperHell = 0;
        }
      } else {
        if (_Helligkeit == 0) {
          return;
        } else if (_Helligkeit > 5) {
          _Helligkeit = _Helligkeit - 5;
        } else {
          _Helligkeit = 0;
        }
      }
      D_PRINTF("Neue Helligkeit(-): Norm %d( Super %d)\n",(uint8_t)_Helligkeit, (uint8_t)_SuperHell);
    }
    uint8_t Status(uint8_t ab_hier, bool zeige_status) {
      if (zeige_status) {
        _streifen->SetPixelColor(ab_hier, 0, 0, 0, 127);
        return ab_hier + 1;
      }
      return ab_hier;
    }
    void Tick(uint8_t ab_hier) {
      _streifen->MonoFarbe(_SuperHell, _SuperHell, _SuperHell, _Helligkeit, ab_hier);
    }
  protected:
    Persist<uint8_t> _Helligkeit;
    Persist<uint8_t> _SuperHell;

};

#endif // Modus_Weiss