#include <Arduino.h>

#include "main.h"


#include "Zugangsinfo.h"
#include "main.h"
#include "OTA.h"
#include "WebS.h"
#include "Controler.h"
#include "Knopf.h"
#include "Netzwerk.h"

LichtModi __Modus;
WebS __WebS;
Knopf __Knopf;
Netzwerk __Netzwerk;
OTA __OTA;

void setup() {
  // LEDs konfigurieren
  //  pinMode(LED_BUILTIN, OUTPUT);

  // Seriellen Output enablen
  D_BEGIN(57600);
  //delay(5000);
  D_PRINTLN("Starte...");

  __Modus.Beginn();
  D_PRINTLN("---------- LEDs ");

  __Netzwerk.Beginn();
  __Netzwerk.Start();
  D_PRINTLN("---------- Wifi ");

  __WebS.Beginn();
  D_PRINTLN("---------- Webserver ");

  // OTA Initialisieren
  __OTA.Beginn();
  __OTA.Bereit();
  D_PRINTLN("---------- OTA ");

  // Knopf Initialisieren
  __Knopf.Beginn();
  __Knopf.Zaehler(0);
  D_PRINTLN("---------- Knopf ");

  D_PRINTLN("setup fertig");
}


int32_t last_decoder_val = 0;
void loop() {
  __Netzwerk.Tick();
  if (__Netzwerk.istVerbunden()) {
    __OTA.Tick();
    if (__OTA.Laeuft()) // Falls ein Netzwerk-Update laeuft, mach nichts anderes
      return;
    __WebS.Tick();
  }
  __Modus.Tick();

  switch (__Knopf.Status()) {
    case Knopf::kurz: { // Knopf gedrückt
        D_PRINTLN("Knopf kurz");
        __Modus.Next_PlusMinus();
        break;
      }
    case Knopf::wert: {  // Knopf gedreht
        int32_t z = __Knopf.Zaehler();
        D_PRINTF("Encoder=<% 3d> \n", z);
        if (z > last_decoder_val)
          __Modus.Plus();
        else
          __Modus.Minus();
        last_decoder_val = z;
        break;
      }
    case Knopf::wert_kurz: {  // Knopf gedrueckt und gedreht
        int32_t z = __Knopf.Zaehler();
        D_PRINTF("Encoder=<% 3d> gedrueckt\n", z);
        if (z > last_decoder_val)
          __Modus.Next_Modus();
        else
          __Modus.Prev_Modus();
        last_decoder_val = z;
        break;
      }
    case Knopf::lang: {
        if (__Netzwerk.istAn()) {
          D_PRINTLN("lang: schalte WiFi aus");
          __Netzwerk.Stop();
        } else {
          D_PRINTLN("lang: schalte WiFi an");
          __Netzwerk.Start();
        }
        break;
      }
    case Knopf::nix:
    default:
      break;
  }
}
