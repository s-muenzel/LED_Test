#ifndef _KNOPF
#define _KNOPF

#define LANG	  5000	// [ms] ab welcher Dauer zaehlt der Tastendruck als "lang"	

class Knopf {
  public:
    Knopf();

    void Beginn();

    typedef enum _Event {
      nix,    // keine Aenderung
      kurz,   // Knopf gedrueckt (1x)
      lang,   // Knopf lang gedrueckt
      wert,   // Drehzaehler
      wert_kurz // Drehzaehler & gedrueckt
    } _Event_t;

    _Event_t Status();

    void Zaehler(int16_t zaehler);
    int16_t Zaehler();

    uint8_t WieLang();

  private:

    long    _Start; 		// [ms] - wann wurde der Knopf gedrueckt
    bool    _Kurz;			// wurde ein kurz bereits gemeldet?
    uint8_t _Lang;			// bei _Lang wird mitgezaehlt

    int16_t _Zaehler;   // Zaehler

};

#endif // _KNOPF
