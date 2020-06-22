#ifndef _LED_Streifen
#define _LED_Streifen

class LED_Streifen {
  public:
    LED_Streifen();

    void Beginn(uint16_t n_Leds);
    void Bereit();

    void MonoFarbe(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t erste = 0);
    void SetPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void Show();

    uint16_t numPixels() {
      return _n_Pixel;
    }

  private:
    uint16_t _n_Pixel;
};

#endif // _LED_Streifen
