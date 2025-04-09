#ifndef Heater_h
#define Heater_h

#include <Arduino.h>

class Heater {
  public:
    Heater(int heaterPin, int fanTogglePin_heater);
    Heater(int heaterPin, int fanTogglePin_heater, int fanPwmPin_heater);

    void toggleHeater(bool onOff);
    void toggleHeaterFan(bool onOff);
    void changeFanSpeed(int percent);

  private:
    int _heaterPin;
    int _fanTogglePin_heater;
    int _fanPwmPin_heater;
};



#endif

