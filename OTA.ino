#include <ArduinoOTA.h>
#include <WiFiUdp.h>

#include "main.h"

#include "OTA.h"
#include "Zugangsinfo.h"

OTA::OTA() {
  _OTA_An = false;
}

void OTA::Beginn() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(HostName);
  // No authentication by default
  //  ArduinoOTA.setPassword("...");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash(ota_hash);
  ArduinoOTA.setRebootOnSuccess(true);
  ArduinoOTA.onStart([]() {
    D_PRINTLN("Start updating ");
  });
  ArduinoOTA.onEnd([]() {
    D_PRINTLN("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    D_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    D_PRINTF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      D_PRINTLN("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      D_PRINTLN("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      D_PRINTLN("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      D_PRINTLN("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      D_PRINTLN("End Failed");
    }
  });

}

void OTA::Bereit() {
  _OTA_An = true;
  ArduinoOTA.begin();
};

void OTA::Tick() {
  if (_OTA_An) {
    ArduinoOTA.handle();
  }
};
