#ifndef _NETZWERK
#define _NETZWERK

class Netzwerk {
  public:
    Netzwerk();
    void Beginn();
    void Start();
    void Stop();
    void Tick();
    bool istAn() { return Status_An; };
    bool istVerbunden();
  private:
  bool Status_An;
};

#endif // _NETZWERK
