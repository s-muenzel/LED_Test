#include <Arduino.h>

#include "main.h"


#include "Zugangsinfo.h"
#include "main.h"
//#include "OTA.h"
#include "WebS.h"
#include "Controler.h"
#include "Knopf.h"
#include "Netzwerk.h"

LichtModi __Modus;
WebS __WebS;
Knopf __Knopf;
Netzwerk __Netzwerk;
///OTA __OTA;

void setup() {
  // LEDs konfigurieren
  pinMode(LED_BUILTIN, OUTPUT);

  // Seriellen Output enablen
  D_BEGIN(115200);
  delay(5000);
  D_PRINT("Starte...");

  __Modus.Beginn();
  D_PRINT(" LEDs ok");

  __Netzwerk.Beginn();
  D_PRINT(" Wifi ok");

  __WebS.Beginn();
  D_PRINT(" Webserver laeuft");

  // OTA Initialisieren
  ///  __OTA.Beginn();
  ///  __OTA.Bereit();
  ///  D_PRINT(" OTA vorbereitet");

  // Knopf Initialisieren
  __Knopf.SetzeBereich(-32767,32767);
  __Knopf.Beginn();
  __Knopf.Zaehler(0);
  D_PRINTF(" Knopf vorbereitet");
}


unsigned long last_led_update = 0;
int32_t last_decoder_val = 0;
void loop() {
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
    case Knopf::wert_kurz: {  // Knopf gedreht
        int32_t z = __Knopf.Zaehler();
        D_PRINTF("Encoder=<% 3d> \n", z);
        if (z > last_decoder_val)
          __Modus.Next_Modus();
        else
          __Modus.Prev_Modus();
        last_decoder_val = z;
        break;
      }
    case Knopf::lang: {
        D_PRINTF("lang WiFi an / aus\n");
        if (__Netzwerk.istAn()) {
          __Netzwerk.Stop();
        } else {
          __Netzwerk.Start();
        }
        break;
      }
    case Knopf::nix:
    default:
      break;
  }

  if (millis() - last_led_update > 50) { // alle 20ms LEDs ansteuern (--> 20Hz)
    delay(1); // --> gibt kurz die Kontrolle ab, dann sind Hintergrundsaufgaben seltener im Weg
    __Modus.Tick();
    last_led_update = millis();
  }

  __Netzwerk.Tick();

  if (__Netzwerk.istVerbunden()) {
    __WebS.Tick();
    ///    __OTA.Tick();
  }
}
