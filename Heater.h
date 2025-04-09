#ifndef Heater_h
#define Heater_h

#include <Arduino.h>

class Heater {
  public:
    Heater(int heaterPin, int fanTogglePin);
    Heater(int heaterPin, int fanTogglePin, int fanPwmPin);

    void toggleHeater(bool onOff);
    void toggleFan(bool onOff);
    void changeFanSpeed(int percent);

  private:
    int _heaterPin;
    int _fanTogglePin;
    int _fanPwmPin;
};



#endif

