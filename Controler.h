#ifndef _LichtModi
#define _LichtModi

class LichtModi {
  public:
    LichtModi();

    void Beginn();
    void Bereit();

    void Tick();

    typedef enum { Aus, Weiss, Farbe, Verlauf, Verlauf2 } Modi;

    void Set_Modus(Modi mode, bool commit = false);
    void Set_Modus(const char*mode, bool commit = false);
    void Next_Modus();
    void Prev_Modus();
    Modi Get_Modus();
    const char* Get_Modus_Name();
    void Next_PlusMinus();
    void Plus();
    void Minus();
    void Set_Helligkeit(uint8_t h, bool commit = false);
    uint8_t Get_Helligkeit();
    void Set_Farbe1(uint32_t f, bool commit = false);
    uint32_t Get_Farbe1();
    void Set_Farbe2(uint32_t f, bool commit = false);
    uint32_t Get_Farbe2();
    void Set_Speed(uint16_t s, bool commit = false);
    uint16_t Get_Speed();
    void Set_n_Leds(uint16_t n, bool commit = false);
    uint16_t Get_n_Leds();
    void Set_Brightness(uint8_t b, bool commit = false);
    uint8_t Get_Brightness();
    void Commit();

  private:

    Modi _Modus;
    uint8_t _Helligkeit;
    uint32_t _Farbe1;
    float _h1, _s1, _v1;
    uint32_t _Farbe2;
    float _h2, _s2, _v2;
    uint16_t _Speed;

    uint16_t _n_Leds;
    uint8_t _Brightness;

    void Tick_Verlauf();
    void Tick_Verlauf2();

    uint8_t PlusMinus_Mode;
};

#endif // _LichtModi
