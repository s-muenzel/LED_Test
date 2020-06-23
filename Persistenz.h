#ifndef _Persistenz
#define _Persistenz

#include <EEPROM.h>

#include "main.h"

//#define EEPROM_WRITE_TIMER 6000000 // 100 Minute Timeout :-) , danach speichern
#define EEPROM_WRITE_TIMER 60000 // eine Minute Timeout, danach speichern

class EEPROM_Master;

class Persistent {
  public:
    Persistent() {
      _p = 0;
      _master = NULL;
    };
    virtual void Save();
    virtual void Load();
    virtual void Reset();
  protected:
    uint32_t _p;
    EEPROM_Master* _master;
};

class EEPROM_Master {
  public:
    EEPROM_Master() {
      next_save = 0;
      next_pos = 0;
      _Liste = NULL;
    }
    uint32_t add(Persistent *v, uint32_t s) {
      Eintrag *tmp = new Eintrag;
      tmp->wert = v;
      tmp->next = _Liste;
      _Liste = tmp;
      uint32_t cur_pos = next_pos;
      next_pos += s;
      return cur_pos;
    }
    void Beginn() { // erst aufrufen wenn alle Werte bekannt sind
      EEPROM.begin(next_pos);
      Eintrag *tmp = _Liste;
      while (tmp != NULL) {
        tmp->wert->Load();
        tmp = tmp->next;
      }
    }
    void ResetAll() {
      Eintrag *tmp = _Liste;
      while (tmp != NULL) {
        tmp->wert->Reset();
        tmp = tmp->next;
      }
      next_save = 1; // Dirty, Jetzt
      Tick(); // Speichern
    }
    void Dirty() {
      next_save = millis() + EEPROM_WRITE_TIMER;
    }
    void Tick(bool now = false) {
      if (now)
        next_save = 1;
      if ((next_save != 0) && (next_save < millis())) {
        Eintrag *tmp = _Liste;
        while (tmp != NULL) {
          tmp->wert->Save();
          tmp = tmp->next;
        }
        EEPROM.commit();
        next_save = 0;
      }
    }
  protected:
    uint32_t next_save;
    uint32_t next_pos;
    struct Eintrag
    {
      Persistent *wert;
      struct Eintrag *next;
    };
    Eintrag* _Liste;
};

template <class p_typ> class Persist : Persistent {
  public:
    Persist() {
    };
    void Init(EEPROM_Master * m, p_typ d = 0) {
      _master = m;
      _v = _d = d;
      _p = m->add(this, sizeof(_v));
    };
    void Save() {
      EEPROM.put(_p, _v);
      D_PRINTF("Speichere an Stelle %x ",_p); D_PRINTLN(_v);
    };
    void Load() {
      EEPROM.get(_p, _v);
      D_PRINTF("Lade von Stelle %x ",_p); D_PRINTLN(_v);
    };
    void Reset() {
      _v = _d;
    }
    void operator=(p_typ v) {
      _v = v;
      _master->Dirty();
    };
    operator p_typ() {
      return _v;
    };
  protected:
    p_typ _v; // actual
    p_typ _d; // default
};

#endif // _Persistenz
