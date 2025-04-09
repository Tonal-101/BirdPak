#include <stdint.h>
#ifndef Temperature_Sensor_h
#define Temperature_Sensor_h

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

class Temperature_Sensor {
  public:
    Temperature_Sensor(uint8_t sensorAddress);
    Temperature_Sensor(uint8_t sensorAddress, int highTempAlarm, int lowTempAlarm);

  private:
    void _initializeSensors();
    int _highTempAlarm;
    int _lowTempAlarm;
    uint8_t _sensorAddress;
};

#endif