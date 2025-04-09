#include <stdint.h>
#ifndef Temperature_Sensor_h
#define Temperature_Sensor_h

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

class Temperature_Sensor {
  public:
    Temperature_Sensor();
    Temperature_Sensor(uint8_t sensorAddress[8]);
    Temperature_Sensor(uint8_t sensorAddress[8], int highTempAlarmF, int lowTempAlarmF);
    void initializeSensors();
    void requestTemperatures();
    int getTempF();
    int getTempC();
    int getTempK();

    int getHighTempThreshold();
    int getLowTempThreshold();

  private:

    int _highTempAlarm;
    int _lowTempAlarm;
    uint8_t _sensorAddress[8];

    OneWire oneWire;
    DallasTemperature sensors;
};

#endif
