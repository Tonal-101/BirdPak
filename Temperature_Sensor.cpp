#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

uint8_t _sensorAddress;
int _highTempAlarm;
int _lowTempAlarm;

Temperature_Sensor::TemperatureSensor(uint8_t sensorAddress) {
  _sensorAddress = sensorAddress;
  sensors.begin();
  _initializeSensors();
}

Temperature_Sensor::Temperature_Sensor(uint8_t sensorAddress, int highTempAlarm, int lowTempAlarm) {
  _sensorAddress = sensorAddress;
  _highTempAlarm = highTempAlarm;
  _lowTempAlarm = lowTempAlarm;

  _initializeSensors();
}

void Temperature_Sensor::_initializeSensors() {
  //~~~~~~~~~~~~~[ Sensor setup ]~~~~~~~~~~~~~//
  // Data wire is plugged into port 4 on the Arduino
  #define ONE_WIRE_BUS 4
  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  OneWire oneWire(ONE_WIRE_BUS);

  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors(&oneWire);

  int numberOfDevices; // Number of temperature devices found
  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

  numberOfDevices = sensors.getDeviceCount();

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