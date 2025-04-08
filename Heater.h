#ifndef Heater_h
#define Heater_h

#include <Arduino.h>

class Heater {
  public:
    Heater(int heaterPin, int fanPin);
    void toggleHeater(bool onOff);
    void toggleFan(bool onOff);

  private:
    int _heaterPin;
    int _fanPin;
};



#endif

