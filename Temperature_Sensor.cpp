#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "Temperature_Sensor.h"

#define ONE_WIRE_BUS 4

// uint8_t _sensorAddress;
int _highTempAlarm;
int _lowTempAlarm;

// Constructor initializer list for OneWire and DallasTemperature
Temperature_Sensor::Temperature_Sensor()
  : oneWire(ONE_WIRE_BUS), sensors(&oneWire) {
  sensors.begin();
  initializeSensors();
}

Temperature_Sensor::Temperature_Sensor(uint8_t sensorAddress[8])
  : oneWire(ONE_WIRE_BUS), sensors(&oneWire) {
  memcpy(_sensorAddress, sensorAddress, 8); // Copy the address
  sensors.begin();
  initializeSensors();
}

Temperature_Sensor::Temperature_Sensor(uint8_t sensorAddress[8], int highTempAlarmF, int lowTempAlarmF)
  : oneWire(ONE_WIRE_BUS), sensors(&oneWire) {
  memcpy(_sensorAddress, sensorAddress, 8); // Copy the address
  _highTempAlarm = highTempAlarmF;
  _lowTempAlarm = lowTempAlarmF;
}

void Temperature_Sensor::initializeSensors() {
  //~~~~~~~~~~~~~[ Sensor setup ]~~~~~~~~~~~~~//
  // // Data wire is plugged into port 4 on the Arduino
  // #define ONE_WIRE_BUS 4
  // // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  // OneWire oneWire(ONE_WIRE_BUS);

  // // Pass our oneWire reference to Dallas Temperature. 
  // DallasTemperature sensors(&oneWire);

  int numberOfDevices; // Number of temperature devices found
  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

  numberOfDevices = sensors.getDeviceCount();

  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      // printAddress(tempDeviceAddress);
      Serial.println();
		} else {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }
}

void Temperature_Sensor::requestTemperatures() {
  sensors.requestTemperatures();
}

int Temperature_Sensor::getTempF() {
  sensors.requestTemperaturesByAddress(_sensorAddress);

  // Wait for DallasTemperature library to finish fetching temperatures
  while(!sensors.isConversionComplete()) { delay(50); }

  return sensors.getTempF(_sensorAddress);
}

int Temperature_Sensor::getTempC() {
  sensors.requestTemperaturesByAddress(_sensorAddress);

  // Wait for DallasTemperature library to finish fetching temperatures
  while(!sensors.isConversionComplete()) { delay(50); }

  return sensors.getTempC(_sensorAddress);
}

int Temperature_Sensor::getTempK() {
  sensors.requestTemperaturesByAddress(_sensorAddress);

  // Wait for DallasTemperature library to finish fetching temperatures
  while(!sensors.isConversionComplete()) { delay(50); }

  return (sensors.getTempC(_sensorAddress) - 273.15);
}

int getHighTempThreshold() {
  return _highTempAlarm;
}

int getLowTempThreshold() {
  return _lowTempAlarm;
}