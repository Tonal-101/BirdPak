#ifndef Cooler_h
#define Cooler_h

#include <Arduino.h>

class Cooler {
  public:
    Cooler(int coolerPin, int fanTogglePin_cooler);
    Cooler(int coolerPin, int fanTogglePin_cooler, int fanPwmPin_cooler);
    void toggleCooler(bool onOff);
    void toggleFan(bool onOff);
    void changeFanSpeed(int percent);

  private:
    int _coolerPin;
    int _fanTogglePin_cooler;
    int _fanPwmPin_cooler;
};



#endif