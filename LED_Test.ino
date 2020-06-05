#include <Arduino.h>

#include "main.h"

#if defined(IST_ESP01) || defined(IST_SONOFF)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif

#ifdef IST_NODEMCU32
#include <WiFi.h>
#include <ESPmDNS.h>
#endif // IST_NODEMCU32

#include "Zugangsinfo.h"
#include "main.h"
//#include "OTA.h"
#include "WebS.h"
#include "Controler.h"
#include "Knopf.h"

LichtModi __Modus;
WebS __WebS;
Knopf __Knopf;
///OTA __OTA;

uint32_t wifi_retry = 0;
#define WIFI_RETRY_DELAY 1000
uint32_t wifi_reset = 0;
#define WIFI_RESET_DELAY 30000
bool wifi_connected = false;

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void setup() {
  // LEDs konfigurieren
  pinMode(LED_BUILTIN, OUTPUT);

  // Seriellen Output enablen
  D_BEGIN(115200);
  delay(5000);
  D_PRINT("Starte...");

  __Modus.Beginn();
  D_PRINT(" LEDs ok");

  // Wifi
  D_PRINT("versuche Wifi ");
#if defined(IST_ESP01) || defined(IST_SONOFF)
  WiFi.hostname(HostName);
#endif
  delay(100);
  D_PRINTLN("vor Wifi.mode ");
  WiFi.mode(WIFI_STA);
  D_PRINTLN("Wifi.mode ");
  WiFi.begin(ssid, password);
  D_PRINTLN("Wifi.begin ");
  // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  for (int i = 0; i < 10; i++) {
    D_PRINT("Schleife "); D_PRINTLN(i);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      delay(1000);
      D_PRINT(".");
    } else {
      D_PRINTF(" Wifi %s (IP Address %s)\n", ssid, WiFi.localIP().toString().c_str());
      wifi_connected = true;
      digitalWrite(LED_BUILTIN, LED_AN);
      break;
    }
  }
  D_PRINT(" (Wifi) done");

  // #ifdef IST_NODEMCU32
  //       WiFi.setHostname(HostName);
  //       if (MDNS.begin(HostName)) {
  //         D_PRINT(" MDNS responder");
  //       }
  // #endif // IST_NODEMCU32  // Webserver konfigurieren
  __WebS.Beginn();
  D_PRINT(" Webserver laeuft");

  // OTA Initialisieren
  ///  __OTA.Beginn();
  ///  __OTA.Bereit();
  ///  D_PRINT(" OTA vorbereitet");

  // Knopf Initialisieren
  __Knopf.SetzeBereich(1, 100);
  __Knopf.Beginn();
  __Knopf.Zaehler(0);
  D_PRINTF(" Knopf vorbereitet");
}

unsigned long last_tick = 0;
void loop() {
  switch (__Knopf.Status()) {
    case Knopf::kurz: { // Knopf gedrückt: Ein- bzw. Ausschalten
        D_PRINTLN("Knopf kurz");
        break;
      }
    case Knopf::wert: {  // Knopf gedreht (geht nur, wenn "Zaehlen(true)")
        int32_t z = __Knopf.Zaehler();
        D_PRINTF("Encoder=<% 3d> \n", z);
        break;
      }
    case Knopf::lang: {
        D_PRINTF("lang <% 3d>\n", __Knopf.Zaehler());
        break;
      }
    case Knopf::nix:
    default:
      break;
  }

  // Primärer Task: LED ansteuern
  //  unsigned long new_tick = millis();
  //  if (new_tick - last_tick <= 21)
  //     __Modus.Tick();
  // else: scheint wieder was anderes gemacht zu haben, auslassen
  // Minimaler Wait (20ms)
  //  last_tick = new_tick;
  //  delay(20);

  delay(1);
  if (millis() - last_tick > 100) {
    __Modus.Tick();
    last_tick = millis();
  }

  // Signalisiere WiFi-Status per LED
  if (WiFi.status() == WL_CONNECTED) {
    if (wifi_connected == false) {
      wifi_connected = true;
      // Connection wieder bekommen, DEBUG Ausgabe
      D_PRINTF(" Wifi %s (IP Address %s)", ssid, WiFi.localIP().toString().c_str());
      digitalWrite(LED_BUILTIN, LED_AN);
    }
    wifi_retry = millis();
    wifi_reset = wifi_retry;
    __WebS.Tick();
    ///    __OTA.Tick();
    return;
  } else {
    if (wifi_connected == true) {
      wifi_connected = false;
      digitalWrite(LED_BUILTIN, LED_AUS);
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (millis() > wifi_retry + WIFI_RETRY_DELAY) {
      D_PRINTF("/%d\r", wifi_retry);
      WiFi.reconnect();
      wifi_retry = millis();
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() > wifi_reset + WIFI_RESET_DELAY) {
      WiFi.disconnect();
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      D_PRINTF(" Wifi reset ");
      wifi_retry = millis();
      wifi_reset = wifi_retry;
    }
  }
}
