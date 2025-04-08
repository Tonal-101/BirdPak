#ifndef Temperature_Sensor_h
#define Temperature_Sensor_h

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

class Temperature_Sensor {
  public:

  private:
    void _initializeSensors();
    
};

#endif