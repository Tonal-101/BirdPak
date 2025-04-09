#ifndef Cooler_h
#define Cooler_h

#include <Arduino.h>

class Cooler {
  public:
    Cooler(int coolerPin, int fanTogglePin);
    Cooler(int coolerPin, int fanTogglePin, int fanPwmPin);
    void toggleCooler(bool onOff);
    void toggleFan(bool onOff);
    void changeFanSpeed(int percent)

  private:
    int _coolerPin;
    int _fanTogglePin;
    int _fanPwmPin;
};



#endif