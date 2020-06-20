#include <Arduino.h>

#include "Knopf.h"


#ifdef IST_NODEMCU32
#define ROTARY_A_PIN  34 // braun
#define ROTARY_B_PIN  32 // 35 // blau
#define KNOPF_PIN     33 // schwarz
#endif
#ifdef IST_ESP32S
// Pins für Rotary Encoder
# define ROTARY_A_PIN  2
# define ROTARY_B_PIN  4
# define KNOPF_PIN     15
#endif

#include <SimpleRotary.h>
SimpleRotary encoder(ROTARY_A_PIN, ROTARY_B_PIN, KNOPF_PIN);

Knopf::Knopf() {
  _Zaehler = 0;   // Zaehler
}

void Knopf::Beginn() {
  pinMode(KNOPF_PIN, INPUT_PULLUP);
  _Start = millis();
  _Kurz = false;
  _Lang = 0;
}

uint32_t Prell_Timer = 0;
Knopf::_Event_t Knopf::Status() {
  // Auswertung: Knopf gedrueckt?
  if (digitalRead(KNOPF_PIN) == HIGH) {
    // Knopf nicht gedrueckt
    _Start = millis();
    _Kurz = false;
    _Lang = 0;
  } else {
    // Knopf gedrueckt
    if (!_Kurz && millis() > Prell_Timer ) {
      // Vorher nicht gedrueckt
      _Kurz = true;
      Prell_Timer = _Start + 250; // max alle x ms ein Klick
      return kurz;
    }
    // Vorher schon gedrueckt: Lang?
    int anzahl_Langs = (millis() - _Start) / LANG;
    if (anzahl_Langs > _Lang) {
      _Lang = anzahl_Langs;
      return lang;
    }
  }

  // Knopf nicht gedrueckt. Gedreht?
  int32_t z;
  // 0 = not turning, 1 = CW, 2 = CCW
  uint8_t i = encoder.rotate();
  switch (i) {
    case 1:
      z = _Zaehler + 1;
      break;
    case 2:
      z = _Zaehler - 1;
      break;
    case 0:
    default:
      z = _Zaehler;
  }
  if (z != _Zaehler) {
    _Zaehler = z;
    if (_Kurz)
      return wert_kurz;
    else
      return wert;
  } else {
    // kein neuer Wert
    return nix;
  }
}

void Knopf::Zaehler(int16_t zaehler) {
  _Zaehler = zaehler;
}

int16_t Knopf::Zaehler() {
  return _Zaehler;
}

uint8_t Knopf::WieLang() {
  return _Lang;
}
