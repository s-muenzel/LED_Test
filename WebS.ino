#include "main.h"
#include "Zugangsinfo.h"
#include "WebS.h"

#include <FS.h>

#ifdef ESP8266 // defined(IST_ESP01) || defined(IST_SONOFF)
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif // IST_SONOFF

#ifdef ESP32
#include <WebServer.h>
#include <SPIFFS.h>
WebServer server(80);
#endif // ESP32

bool __Admin_Mode_An;

///////// Hilfsfunktionen
//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + " B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + " KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + " MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
  }
}
//Stunde aus Text
uint8_t parseZeit_Stunde(String s) {
  return min(23, max(0, int(s.toInt())));
}
//Minute aus Text
uint8_t parseZeit_Minute(String s) {
  return min(59, max(0, int(s.substring(3).toInt())));
}


void handleRoot() {
  D_PRINTLN("handleRoot");
  if (SPIFFS.exists("/top.htm")) {
    File file = SPIFFS.open("/top.htm", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "file error");
  }
}

void handleControl() {
  D_PRINTLN("handleControl");
  if (SPIFFS.exists("/Control.htm")) {
    File file = SPIFFS.open("/Control.htm", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "file error");
  }
}

int StrToHex(const char c)
{
  switch (c) {
    default:
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A':
    case 'a': return 10;
    case 'B':
    case 'b': return 11;
    case 'C':
    case 'c': return 12;
    case 'D':
    case 'd': return 13;
    case 'E':
    case 'e': return 14;
    case 'F':
    case 'f': return 15;
  }
}

void handleSet_Values() {
  D_PRINTLN("handleSet_Values");
  //  String Erg("Werte: ");
  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i) == "Modus") {
      __Modus.Set_Modus(server.arg(i).c_str());
      if (server.arg(i) == "Aus") {
        __Modus.Set_Modus(LichtModi::Aus);
      } else if (server.arg(i) == "Weiss") {
        __Modus.Set_Modus(LichtModi::Weiss);
      } else if (server.arg(i) == "Farbe") {
        __Modus.Set_Modus(LichtModi::Farbe);
      } else if (server.arg(i) == "Verlauf") {
        __Modus.Set_Modus(LichtModi::Verlauf);
      }
    } else if (server.argName(i) == "Helligkeit1") {
      ///     __Modus.Set_Helligkeit(server.arg(i).toInt());
    } else if (server.argName(i) == "Farbe1") {
      uint8_t r = StrToHex(server.arg(i).charAt(1)) * 16;
      r += StrToHex(server.arg(i).charAt(2));
      uint8_t g = StrToHex(server.arg(i).charAt(3)) * 16;
      g += StrToHex(server.arg(i).charAt(4));
      uint8_t b = StrToHex(server.arg(i).charAt(5)) * 16;
      b += StrToHex(server.arg(i).charAt(6));
      ///     __Modus.Set_Farbe1(r * 256 * 256 + g * 256 + b);
    } else if (server.argName(i) == "Farbe2") {
      uint8_t r = StrToHex(server.arg(i).charAt(1)) * 16;
      r += StrToHex(server.arg(i).charAt(2));
      uint8_t g = StrToHex(server.arg(i).charAt(3)) * 16;
      g += StrToHex(server.arg(i).charAt(4));
      uint8_t b = StrToHex(server.arg(i).charAt(5)) * 16;
      b += StrToHex(server.arg(i).charAt(6));
      ///     __Modus.Set_Farbe2(r * 256 * 256 + g * 256 + b);
    } else if (server.argName(i) == "Speed") {
      ///     __Modus.Set_Speed(server.arg(i).toInt());
    }
  }
  __Modus.Commit();
  server.sendHeader("Location", "/Control");
  server.send(303, "text/html", "Location:/Control");
}

void handleCSS() {
  D_PRINTLN("handleCSS");
  if (SPIFFS.exists("/style.css")) {
    File file = SPIFFS.open("/style.css", "r");
    server.streamFile(file, "text/css");
    file.close();
  } else {
    server.send(404, "text/plain", "file error");
  }
}

void handleReset() {
  D_PRINTF("Webaufruf /Reset um %lu\n", millis());
  if (__Admin_Mode_An) {
    D_PRINTF("Resette JETZT\n");
    server.send(200, "text/plain", "Resetting - reload now");
    delay(1000);
    ESP.restart();
  } else {
    D_PRINTLN("KEIN ADMIN MODE - tue nix\n");
    server.send(403, "text/plain", "Kein Admin-Mode!");
  }
}

void handleAdmin() {
  D_PRINTF("Webaufruf /Admin um %lu\n", millis());
  if (!server.authenticate(admin_user, admin_pw)) {
    server.requestAuthentication(DIGEST_AUTH, "Admin-Mode", "Admin Mode failed");
  } else {
    // User authorisiert, Admin_Mode anschalten
    __Admin_Mode_An = true;
    //    __OTA.Bereit();
    server.send(200, "text/plain", "Admin-Mode eingeschaltet");
  }
}


void handleStatus() {
  char temp[1000];
  snprintf(temp, 1000,
           "{ \"Zeit\" : \"%lu\", \"Admin\" : %d}", millis(), __Admin_Mode_An);
  server.send(200, "application/json", temp);
}

void handleModiListe() {
  char temp[1000];
  snprintf(temp, 1000, "{ \"Modi\":[ "\
           "{\"Modus\":\"Aus\",   \"H\": \"0\", \"F1\": \"0\", \"F2\": \"0\", \"S\": \"0\" },"\
           "{\"Modus\":\"Weiss\", \"H\": \"1\", \"F1\": \"0\", \"F2\": \"0\", \"S\": \"0\" },"\
           "{\"Modus\":\"Farbe\", \"H\": \"0\", \"F1\": \"1\", \"F2\": \"0\", \"S\": \"0\" },"\
           "{\"Modus\":\"Verlauf\", \"H\": \"0\", \"F1\": \"1\", \"F2\": \"1\", \"S\": \"1\" },"\
           "{\"Modus\":\"Verlauf2\", \"H\": \"0\", \"F1\": \"1\", \"F2\": \"1\", \"S\": \"1\" }"\
           "], \"Modus\": \"%s\", \"H\": \"%d\", \"F1\": \"#%06x\", \"F2\": \"#%06x\", \"S\": \"%d\"}",
           ///           __Modus.Get_Modus_Name(), __Modus.Get_Helligkeit(), __Modus.Get_Farbe1(), __Modus.Get_Farbe2(), __Modus.Get_Speed());
           __Modus.Get_Modus_Name(), "0", "0", "0", "0");
  server.send(200, "application/json", temp);
}

void handleKontrol() {
  D_PRINTLN("handleKontrol");
  if (SPIFFS.exists("/Kontrol.htm")) {
    File file = SPIFFS.open("/Kontrol.htm", "r");
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "file error");
  }
}

void handleLade_Konfig() {
  char temp[1000];
  snprintf(temp, 1000, "{ \"Admin\": \"%d\", \"N_LEDs\": \"%d\", \"BRIGHT\": \"%d\" }",
           __Admin_Mode_An ? 1 : 0, __Modus.Get_n_Leds(), "0");
  server.send(200, "application/json", temp);
}

void handleSet_Konfig() {
  D_PRINTLN("handleSet_Konfig");
  String msg = "Rebooting now, neue Werte: ";
  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i) == "N_LEDs") {
      msg += "N_LEDs:";
      msg += server.arg(i).toInt();
      __Modus.Set_n_Leds(server.arg(i).toInt());
    }
//    if (server.argName(i) == "BRIGHT") {
//      msg += "BRIGHT:";
//      msg += server.arg(i).toInt();
//      __Modus.Set_Brightness(server.arg(i).toInt());
//    }
  }
  __Modus.Commit();
  server.send(200, "text/plain", msg);
  ESP.restart();
}

void handleFavIcon() {
  D_PRINTLN("handleFavIcon");
  if (SPIFFS.exists("/favicon.ico")) {
    File file = SPIFFS.open("/favicon.ico", "r");
    server.streamFile(file, "image/x-icon");
    file.close();
  } else {
    server.send(404, "text/plain", "file error");
  }
}

void handleNotFound() {
  D_PRINTF("Webaufruf - unbekannte Seite %s um %lu\n", server.uri().c_str(), millis());
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ":" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

File fsUploadFile;
void handleHochladen() {
  if (__Admin_Mode_An) {
    if (server.uri() != "/Hochladen") {
      return;
    }
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      String filename = upload.filename;
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      D_PRINT("handleHochladen Name: "); D_PRINTLN(filename);
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      //D_PRINT("handleHochladen Data: "); D_PRINTLN(upload.currentSize);
      if (fsUploadFile) {
        fsUploadFile.write(upload.buf, upload.currentSize);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (fsUploadFile) {
        fsUploadFile.close();
      }
      D_PRINT("handleHochladen Groesse: "); D_PRINTLN(upload.totalSize);
    }
  }
}

void handleLoeschen() {
  if (__Admin_Mode_An) {
    if ((server.args() == 0) || (server.argName(0) != "datei")) {
      return server.send(500, "text/plain", "BAD ARGS");
    }
    String path = server.arg(0);
    D_PRINTLN("handleLoeschen: " + path);
    if (path == "/") {
      return server.send(500, "text/plain", "BAD PATH");
    }
    if (!SPIFFS.exists(path)) {
      return server.send(404, "text/plain", "FileNotFound");
    }
    SPIFFS.remove(path);
    server.sendHeader("Location", "/Dateien");
    server.send(303, "text/html", "Location:/Dateien");
    path = String();
  } else {
    D_PRINTLN("KEIN ADMIN MODE - tue nix\n");
    server.send(403, "text/plain", "Kein Admin-Mode!");
  }
}


void handleDateien() {
  D_PRINTLN("Seite handleDateien");

  String output;
  output = "<html><head><meta charset='UTF-8'><link rel='stylesheet' type='text/css' href='style.css'></head><body>";
  if (__Admin_Mode_An) {
    output += String("<form action='/Hochladen' method='post' enctype='multipart/form-data'><span><div class='Tag'><input type='file' name='name'></div></span><span><input class='button' type='submit' value='Upload'></span></form>");
  }

#ifdef ESP8266
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    File entry = dir.openFile("r");
#endif // ESP8266
#ifdef ESP32
    File dir = SPIFFS.open("/");
    File entry = dir.openNextFile();
    while (entry) {
#endif // ESP32
      if (__Admin_Mode_An) {
        output += String("<form action='/Loeschen' method='post'>");
      }
      output += String("<span><div class='Tag'><span>");
      output += entry.name() + String("</span><span class='Rechts'>") + formatBytes(entry.size());
      if (__Admin_Mode_An) {
        output += String("</span><input type='text' style='display:none' name='datei' value='") + entry.name();
        output += String("'></div></span><span><input class='button' type='submit' value='l&ouml;schen'></span></form>");
      } else {
        output += String("</span></div>");
      }
      D_PRINTF("File '%s', Size %d\n", entry.name(), entry.size());
      entry.close();
#ifdef ESP32
      entry = dir.openNextFile();
#endif // ESP32
    }
    output += "</body></html>";
    server.send(200, "text/html", output);
  }
  ///////////

  WebS::WebS() {
    __Admin_Mode_An = false;
  }

  void WebS::Beginn() {
    if (!SPIFFS.begin()) {
      D_PRINTLN("Failed to mount file system");
    }
#ifdef DEBUG_SERIAL
    {
#ifdef ESP8266
      Dir dir = SPIFFS.openDir("/");
      while (dir.next()) {
        File entry = dir.openFile("r");
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        D_PRINTF("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
#endif // ESP8266
#ifdef ESP32
      File dir = SPIFFS.open("/");
      File entry = dir.openNextFile();
      while (entry) {
        D_PRINTF("FS File: %s, size: %s\n", entry.name(), formatBytes(entry.size()).c_str());
        entry = dir.openNextFile();
      }
#endif // ESP32
    }
#endif // DEBUG_SERIAL

    server.on("/",              handleRoot);          // Top - lädt top Seite
    server.on("/Control",       handleControl);       // Anzeige Auswahl der Modi und Farben
    server.on("/Set_Values",    handleSet_Values);    // Post-Methode: neue Werte...
    server.on("/Status",        handleStatus);        // liefert lokale Zeit, Admin- und Aktiv-Status per JSON
    server.on("/ModiListe",     handleModiListe);     // liefert alle moeglichen Modi, die Sichtbarkeit der Felder und den aktuellen Status
    server.on("/Kontrol",       handleKontrol);       // Einstellungen (Admin, +Reboot noetig)
    server.on("/Lade_Konfig",   handleLade_Konfig);   // liefert Admin- und Konfigurationswerte
    server.on("/Set_Konfig",    handleSet_Konfig);    // Post-Methode: neue Werte, erzwingen Reboot...
    server.on("/Reset",         handleReset);         // Neustart, nötig wenn z.B. Hostname geändert wurde oder Admin-Mode zurückgesetzt werden soll
    server.on("/Admin",         handleAdmin);         // Admin-Mode einschalten (erforder Authentifizierung)

    server.on("/Dateien",       handleDateien);       // Datei-Operationen (upload, delete)
    server.on("/Loeschen",      handleLoeschen);      // Delete (spezifische Datei)
    server.on("/Hochladen", HTTP_POST, []() { //first callback is called after the request has ended with all parsed arguments
      if (__Admin_Mode_An) {
        server.sendHeader("Location", "/Dateien");
        server.send(303, "text/html", "Location:/Dateien");
      } else {
        server.send(403, "text/plain", "Kein Admin-Mode!");
      }
    },  handleHochladen);        //second callback handles file uploads at that location
    server.on("/favicon.ico",   handleFavIcon);       // liefert das Favicon.ico
    server.on("/style.css",     handleCSS);           // liefert das Stylesheet
    server.onNotFound(          handleNotFound);      // Fallback

    server.begin();
  }


  void WebS::Admin_Mode() {
    __Admin_Mode_An = true;
  }

  void WebS::Tick() {
    server.handleClient();
  }
