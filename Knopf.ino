#include <Arduino.h>

#include "Knopf.h"


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


#define USE_SIMPLEROTARY
#ifdef USE_SIMPLEROTARY
#include <SimpleRotary.h>
SimpleRotary encoder(ROTARY_A_PIN, ROTARY_B_PIN, KNOPF_PIN);
#endif // USE_SIMPLEROTARY

//#define USE_ENCODERSTEPCOUNTER
#ifdef USE_ENCODERSTEPCOUNTER
#include <EncoderStepCounter.h>
EncoderStepCounter encoder(ROTARY_A_PIN, ROTARY_B_PIN);
void interrupt() {
  encoder.tick();
}
#endif // USE_ENCODERSTEPCOUNTER

//#define USE_ESP32ENCODER
#ifdef USE_ESP32ENCODER
#include <ESP32Encoder.h>
ESP32Encoder encoder;
#endif //  USE_ESP32ENCODER


Knopf::Knopf() {
  _Zaehler = 0;   // Zaehler
  _An = false;
}

void Knopf::Beginn() {
  pinMode(KNOPF_PIN, INPUT_PULLUP);
  _Start = millis();
  _Kurz = false;
  _Lang = 0;
  _An = true;

#ifdef USE_SIMPLEROTARY
  // Nothing to do
#endif // USE_SIMPLEROTARY

#ifdef USE_ENCODERSTEPCOUNTER
  encoder.begin();
  // Initialize interrupts
  attachInterrupt(ROTARY_A_PIN, interrupt, CHANGE);
  attachInterrupt(ROTARY_B_PIN, interrupt, CHANGE);
#endif // USE_ENCODERSTEPCOUNTER

#ifdef USE_ESP32ENCODER
  //  ESP32Encoder::useInternalWeakPullResistors = true;
  encoder.attachSingleEdge(ROTARY_A_PIN, ROTARY_B_PIN);
  encoder.clearCount();
  encoder.setCount(_Zaehler);
  //  encoder.attachHalfQuad(ROTARY_A_PIN, ROTARY_B_PIN);
  //  encoder.attachFullQuad(ROTARY_A_PIN, ROTARY_B_PIN);
#endif //  USE_ESP32ENCODER
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

  // Knopf nicht gedrueckt. Aktiv?
  if (_An) {
    // Gedreht?
    int32_t z;
#ifdef USE_SIMPLEROTARY
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
#else
# ifdef USE_ENCODERSTEPCOUNTER
    z = _Zaehler + encoder.getPosition();
# else
#  ifdef USE_ESP32ENCODER
    z = _Zaehler + encoder.getCount();
#  else
    z = 0;
#  endif // USE_ESP32ENCODER
# endif // USE_ENCODERSTEPCOUNTER
#endif // USE_SIMPLEROTARY

    if (z != _Zaehler) {
      _Zaehler = z;
#ifdef USE_SIMPLEROTARY
#endif // USE_SIMPLEROTARY
#ifdef USE_ENCODERSTEPCOUNTER
      encoder.reset();
#endif // USE_ENCODERSTEPCOUNTER
#ifdef USE_ESP32ENCODER
      encoder.setCount(0);
#endif // USE_ESP32ENCODER
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
  _Zaehler = zaehler;
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
