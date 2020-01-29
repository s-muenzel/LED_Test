#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "Zugangsinfo.h"
#include "main.h"
#include "OTA.h"
#include "WebS.h"
#include "Controler.h"

LichtModi __Modus;
WebS __WebS;
OTA __OTA;

void setup() {
  // Seriellen Output enablen
  D_BEGIN(115200);
  D_PRINT("Starte...");

  // Wifi
  WiFi.hostname("Lichtwand");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    D_PRINTLN("Keine Wifi-Verbindung! Neustart in 5 Sekunden...");
    delay(5000);
    ESP.restart();
  }
  D_PRINTF(" Wifi %s (IP Address %s)", ssid, WiFi.localIP().toString().c_str());

  if (MDNS.begin("Lichtwand")) {
    D_PRINT(" MDNS responder");
  }

  // Webserver konfigurieren
  __WebS.Beginn();
  D_PRINT(" Webserver laeuft");

  // OTA Initialisieren
  __OTA.Beginn();
  __OTA.Bereit();
  D_PRINT(" OTA vorbereitet");

  // LEDs konfigurieren
  __Modus.Beginn();
  D_PRINT(" LEDs ok");

}

void loop() {
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.reconnect();
  }
  __WebS.Tick();
  __OTA.Tick();
  __Modus.Tick();
}
