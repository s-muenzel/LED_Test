#include <Arduino.h>

#include <ESP32Encoder.h>

#define USE_ESP32ENCODER
#ifdef USE_ESP32ENCODER
#include "Knopf.h"
#endif //  USE_ESP32ENCODER

#ifdef IST_NODEMCU32
#define ROTARY_A_PIN  34 // braun
#define ROTARY_B_PIN  32 // 35 // blau
#define KNOPF_PIN     33 // schwarz
#endif
#ifdef IST_ESP32S
// Pins für Rotary Encoder
# define ROTARY_A_PIN  4
# define ROTARY_B_PIN  2
# define KNOPF_PIN     15
#endif

#ifdef USE_ESP32ENCODER
ESP32Encoder encoder;
#endif //  USE_ESP32ENCODER

Knopf::Knopf() {
  _Minimum = 0;   // Wertebereich
  _Maximum = 100;   // Wertebereich
  _Zaehler = _Minimum;   // Zaehler
  _An = false;
}

void Knopf::Beginn() {
  pinMode(KNOPF_PIN, INPUT_PULLUP);
  _Start = millis();
  _Kurz = false;
  _Lang = 0;
  _An = true;

#ifdef USE_ESP32ENCODER
  ESP32Encoder::useInternalWeakPullResistors = true;
  encoder.clearCount();
  encoder.setCount(_Zaehler);
  //  encoder.attachHalfQuad(ROTARY_A_PIN, ROTARY_B_PIN);
  //  encoder.attachFullQuad(ROTARY_A_PIN, ROTARY_B_PIN);
  encoder.attachSingleEdge(ROTARY_A_PIN, ROTARY_B_PIN);
#endif //  USE_ESP32ENCODER
}

void Knopf::SetzeBereich(int16_t minimum, int16_t maximum) {
  _Minimum = minimum;   // Wertebereich
  _Maximum = maximum;   // Wertebereich
  if (_Zaehler < _Minimum) {
    _Zaehler = _Minimum;
#ifdef USE_ESP32ENCODER
    encoder.setCount(_Zaehler);
#endif //  USE_ESP32ENCODER
  }
  if (_Zaehler > _Maximum) {
    _Zaehler = _Maximum;
#ifdef USE_ESP32ENCODER
    encoder.setCount(_Zaehler);
#endif //  USE_ESP32ENCODER
  }
}


Knopf::_Event_t Knopf::Status() {
  // Auswertung: Knopf gedrueckt?
  if (digitalRead(KNOPF_PIN) == HIGH) {
    // Knopf nicht gedrueckt
    _Start = millis();
    _Kurz = false;
    _Lang = 0;
  } else {
    // Knopf gedrueckt
    if (!_Kurz) {
      // Vorher nicht gedrueckt
      _Kurz = true;
      return kurz;
    }
    // Vorher schon gedrueckt: Lang?
    int anzahl_Langs = (millis() - _Start) / LANG;
    if (anzahl_Langs > _Lang) {
      _Lang = anzahl_Langs;
      return lang;
    }
  }

  // Knopf nicht gedrueckt. Aktiv?
  if (_An) {
    // Gedreht?
    int32_t z;
#ifdef USE_ESP32ENCODER
    z = encoder.getCount();
#else
    z = 0;
#endif //  USE_ESP32ENCODER
    if (z < _Minimum) {
      z = _Minimum;
#ifdef USE_ESP32ENCODER
      encoder.setCount(z);
#endif //  USE_ESP32ENCODER
    } else if (z > _Maximum) {
      z = _Maximum;
#ifdef USE_ESP32ENCODER
      encoder.setCount(z);
#endif //  USE_ESP32ENCODER
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
  } else {
    // Inaktiv
    return nix;
  }
}

void Knopf::Zaehler(int16_t zaehler) {
  int16_t z = zaehler;
  if (z < _Minimum) {
    z = _Minimum;
  }
  if (z > _Maximum) {
    z = _Maximum;
  }
  _Zaehler = z;
#ifdef USE_ESP32ENCODER
  encoder.setCount(_Zaehler);
#endif // USE_ESP32ENCODER
}

int16_t Knopf::Zaehler() {
  return _Zaehler;
}
void Knopf::Zaehlen(bool an) {
  _An = an;
  if (_An) {
#ifdef USE_ESP32ENCODER
    encoder.resumeCount();
#endif // USE_ESP32ENCODER
  } else {
#ifdef USE_ESP32ENCODER
    encoder.pauseCount();
#endif // USE_ESP32ENCODER
  }
}

uint8_t Knopf::WieLang() {
  return _Lang;
}
