#include <EEPROM.h>
#include "main.h"
#include "Controler.h"
#include "Color_Helper.h"

#define STATUS_DAUER  5000 // 5 sekunden Status anzeigen

#define E_VERSION         1
#define E_N_LEDS        150

LichtModi::LichtModi() {
  Status_Timer = 0;
  last_led_update = 0xFFFFFF00;
  _Modi.push_back(new Modus_Aus(&_Streifen));
  _Modi.push_back(new Modus_Weiss(&_Streifen));
  _Modi.push_back(new Modus_Farbe(&_Streifen));
  _Modi.push_back(new Modus_Verlauf(&_Streifen));
  _Modi.push_back(new Modus_Verlauf2(&_Streifen));
}

void LichtModi::Beginn() {
  D_PRINTLN("LichtModi Beginn");
  _Version.Init(&_E_Master, E_VERSION);
  _n_Leds.Init(&_E_Master, E_N_LEDS);

  _E_Master.Beginn();
  if ((float)_Version != E_VERSION) {
    D_PRINTF("Falsche Version (%d):(%d) , reset\n", E_VERSION, (uint16_t)_Version);
    _E_Master.ResetAll();
  }

  _Modus = Weiss;

  _Streifen.Beginn(_n_Leds);

  last_led_update = 0;
  D_PRINTLN("ENDE LichtModi");
}

void LichtModi::Bereit() {
  _Streifen.Bereit();
}

void LichtModi::Tick() {
  _E_Master.Tick();
  if (millis() > last_led_update + 50 ) { // alle 20ms LEDs ansteuern (--> 20Hz)
    delay(1); // --> gibt kurz die Kontrolle ab, dann sind Hintergrundsaufgaben seltener im Weg
    uint8_t n_status_pixel = 0;
    n_status_pixel = _Modi[_Modus]->Status(n_status_pixel, (millis() < Status_Timer));
    _Modi[_Modus]->Tick(n_status_pixel);
    _Streifen.Show();
    last_led_update = millis();
  }
}

void LichtModi::Set_Modus(Modi mode) {
  _Modus = mode;
  D_PRINTF("Neuer Modus(%d): %s\n", mode, Get_Modus_Name());
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_Modus(const char* mode) {
  _Modus = Aus;
  for (int i = Aus; i < letzter_Modus; i++) {
    if (strcmp(mode, _Modi[i]->Name()) == 0) {
      Set_Modus((Modi)i);
      return;
    }
  }
  D_PRINTF("Set_Modus: keinen Modus gefunden  %s\n", mode);
}

void LichtModi::Next_Modus() {
  Set_Modus((Modi)((_Modus + 1) % letzter_Modus));
}

void LichtModi::Prev_Modus() {
  Modi letzer = letzter_Modus;
  uint8_t i = (_Modus + letzer) - 1;
  Set_Modus((Modi)(i % letzter_Modus));
}

LichtModi::Modi LichtModi::Get_Modus() {
  return _Modus;
}

const char* LichtModi::Get_Modus_Name() {
  return _Modi[_Modus]->Name();
}

void LichtModi::Next_PlusMinus() {
  _Modi[_Modus]->Next_PlusMinus();
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Plus() {
  _Modi[_Modus]->Plus();
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Minus() {
  _Modi[_Modus]->Minus();
  Status_Timer = millis() + STATUS_DAUER;
}

void LichtModi::Set_n_Leds(uint16_t n) {
  _n_Leds = n;
}

uint16_t LichtModi::Get_n_Leds() {
  return _n_Leds;
}
void LichtModi::Commit() {
  _E_Master.Tick(true);
}
