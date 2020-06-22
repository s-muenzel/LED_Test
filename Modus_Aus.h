#ifndef _Modus_Aus
#define _Modus_Aus

#include "Controler.h"

class Modus_Aus : public LichtModus {
  public:
    Modus_Aus(LED_Streifen *streifen): LichtModus(streifen) {};
    const char* Name() {
      return "Aus";
    }
    void Next_PlusMinus() {
    }
    void Plus() {
    }
    void Minus() {
    }
    uint8_t Status(uint8_t ab_hier, bool zeige_status) {
      return ab_hier;
    }
    void Tick(uint8_t ab_hier) {
      _streifen->MonoFarbe(0, 0, 0, 0, ab_hier);
    }
};

#endif // Modus_Aus
