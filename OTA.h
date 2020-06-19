#ifndef _OTA
#define _OTA

class OTA {
  public:
    OTA();

    void Beginn();
    void Bereit();

    void Tick();
    bool Laeuft();

  private:

    bool _OTA_An; // Wurde OTA schon "angeschltet"
//    bool _OTA_Laeuft; // Static, da in Lambda-Funktionen Zugriff erfolgt
};

#endif // _OTA
