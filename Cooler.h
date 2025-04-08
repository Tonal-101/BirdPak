#ifndef Cooler_h
#define Cooler_h

#include <Arduino.h>

class Cooler {
  public:
    Cooler(int coolerPin, int fanPin);
    void toggleCooler(bool onOff);
    void toggleFan(bool onOff);
    void changeFanSpeed(int rpm)

  private:
    int _coolerPin;
    int _fanPin;
};



#endif