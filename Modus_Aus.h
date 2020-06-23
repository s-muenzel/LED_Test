#ifndef _Modus_Aus
#define _Modus_Aus

#include "Controler.h"

class Modus_Aus : public LichtModus {
  public:
    Modus_Aus(LED_Streifen *streifen): LichtModus(streifen) {};
    const char* Name() {
      return "Aus";
    }
    const char* Params() {
      return "{\"Modus\":\"Aus\",  \"Params\":  [ null ] }";
    }
    void SetParam(const char* name, const char* wert) {
      D_PRINTF("Modus Aus: Fehler bei Param: %s - %s\n", name, wert);
      return;
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
