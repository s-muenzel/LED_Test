#ifndef _KNOPF
#define _KNOPF

#define LANG	  1500	// [ms] ab welcher Dauer zaehlt der Tastendruck als "lang"	

class Knopf {
  public:
    Knopf();

    void Beginn();

    void SetzeBereich(int16_t minimum, int16_t maximum);
    int16_t Minimum() { return _Minimum; };
    int16_t Maximum() { return _Maximum; };

    typedef enum _Event {
      nix,    // keine Aenderung
      kurz,   // Knopf gedrueckt (1x)
      lang,   // Knopf lang gedrueckt
      wert    // Entweder Drehzaehler oder "lang"-Zaehler
    } _Event_t;

    _Event_t Status();

    void Zaehler(int16_t zaehler);
    int16_t Zaehler();
    void Zaehlen(bool an);

    uint8_t WieLang();

  private:

    long    _Start; 		// [ms] - wann wurde der Knopf gedrueckt
    bool    _Kurz;			// wurde ein kurz bereits gemeldet?
    uint8_t _Lang;			// bei _Lang wird mitgezaehlt

    int16_t _Minimum;   // Wertebereich
    int16_t _Maximum;   // Wertebereich
    int16_t _Zaehler;   // Zaehler

    bool    _An;        // Zaehler aktiv / passiv
};

#endif // _KNOPF
