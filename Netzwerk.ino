
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#endif // IST_NODEMCU32

#include "main.h"
#include "Zugangsinfo.h"
#include "Netzwerk.h"


uint32_t wifi_retry = 0;
#define WIFI_RETRY_DELAY 1000
uint32_t wifi_reset = 0;
#define WIFI_RESET_DELAY 30000
bool wifi_connected = false;


Netzwerk::Netzwerk() {
  Status_An = false;
}

void Netzwerk::Beginn() {
#ifdef ESP8266
  WiFi.hostname(HostName);
#endif
  WiFi.mode(WIFI_STA);
#ifdef  ESP32 // IST_NODEMCU32
  //  WiFi.setHostname(HostName);
  if (MDNS.begin(HostName)) {
    D_PRINT(" MDNS responder");
  }
#endif // IST_NODEMCU32
}

void Netzwerk::Start() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    D_PRINTF(" Wifi (IP Address %s)\n", WiFi.localIP().toString().c_str());
  }
  wifi_reset = wifi_retry = millis();
  Status_An = true;
}

void Netzwerk::Stop() {
  WiFi.disconnect(true,false);
  Status_An = false;
}

void Netzwerk::Tick() {
  if (!Status_An)
    return;

  unsigned long jetzt = millis();

  if (WiFi.status() == WL_CONNECTED) {
    wifi_reset = wifi_retry = jetzt;
    return;
  }
  
  if (jetzt > wifi_reset + WIFI_RESET_DELAY) {
    WiFi.disconnect(true, false);
    delay(1);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    D_PRINTF(" Wifi reset ");
    wifi_reset = wifi_retry = jetzt;
  }

  if (jetzt > wifi_retry + WIFI_RETRY_DELAY) {
    D_PRINTF("/%d\r", wifi_retry);
    WiFi.reconnect();
    wifi_retry = jetzt;
  }
}

bool Netzwerk::istVerbunden() {
  return Status_An && (WiFi.isConnected());
}
