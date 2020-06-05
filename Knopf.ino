#include <Arduino.h>

#include <ESP32Encoder.h>

#include "Knopf.h"

#define ROTARY_A_PIN  34 // braun
#define ROTARY_B_PIN  32 // 35 // blau
#define KNOPF_PIN     33 // schwarz

ESP32Encoder encoder;

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

  ESP32Encoder::useInternalWeakPullResistors = true;
  encoder.clearCount();
  encoder.setCount(_Zaehler);
  encoder.attachHalfQuad(ROTARY_A_PIN, ROTARY_B_PIN);
}

void Knopf::SetzeBereich(int16_t minimum, int16_t maximum) {
  _Minimum = minimum;   // Wertebereich
  _Maximum = maximum;   // Wertebereich
  if (_Zaehler < _Minimum) {
    _Zaehler = _Minimum;
    encoder.setCount(_Zaehler);
  }
  if (_Zaehler > _Maximum) {
    _Zaehler = _Maximum;
    encoder.setCount(_Zaehler);
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
    int32_t z = encoder.getCount();
    if (z < _Minimum) {
      z = _Minimum;
      encoder.setCount(z);
    } else if (z > _Maximum) {
      z = _Maximum;
      encoder.setCount(z);
    }
    if (z != _Zaehler) {
      _Zaehler = z;
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
  encoder.setCount(_Zaehler);
}

int16_t Knopf::Zaehler() {
  return _Zaehler;
}
void Knopf::Zaehlen(bool an) {
  _An = an;
  if (_An) {
    encoder.resumeCount();
  } else {
    encoder.pauseCount();
  }
}

uint8_t Knopf::WieLang() {
  return _Lang;
}
