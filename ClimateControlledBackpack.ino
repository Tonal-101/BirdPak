#include <Heater.h>
#include <Cooler.h>
#include <Speaker.h>
#include <Temperature_Sensor.h>
#include <Display.h>


// Variables for Temperature Sensors ~~~~~~~~~~~~
  // Sensor Addresses
  uint8_t sensor_current[8] = {0x28, 0xD0, 0xA7, 0x55, 0x00, 0x00, 0x00, 0xE1}; // 64-bit address of DS18B20 temp sensor
  uint8_t sensor_outdoor[8] = {0x28, 0x4A, 0xAC, 0x59, 0x00, 0x00, 0x00, 0xFD};
  uint8_t sensor_heater[8];
  uint8_t sensor_cooler[8];
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Heater ~~~~~~~~~~~~~~~~~~~~~~~~~
  int heaterPin = 5; // output pin that controls ALL HEATERS
  int heater_Temp;   // stores the temperature of the heating unit

  int heaterFanPin = 6;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Cooler ~~~~~~~~~~~~~~~~~~~~~~~~~
  int coolerPin = 7; // output pin that controls ALL COOLERS
  int cooler_Temp;   // stores the temperature of the cooling unit

  int coolerFanPin = 11; // output pin that controls cooler fan (PWM capable)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Speaker ~~~~~~~~~~~~~~~~~~~~~~~~
  int speakerPin   = A1; // output pin that controls the onboard passive buzzer
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// General Temperature Variables ~~~~~~~~~~~~~~~~
#define OFF true
#define ON false

int targetTemp = 71; // default target temp (*F)

int indoorTemp; // current temp inside enclosure
int outdoorTemp; // current temp outside enclosure

// These are used to determine if the display needs to be refreshed
bool targetTempChanged;   // 'true' if current temperature is different from previous temperature
bool indoorTempChanged;   // ^^^
bool outdoorTempChanged;  // ^^^

int  timeToTemp; // estimated time until enclosure reaches the target temperature
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



void setup() {
  Serial.begin(115200);
  
}

void loop() {

}

void updateTimeToTemp() {
  float m = 0.0322;
  float c = 1005;
  int dT = ((targetTemp - 32) * (5/9) + 273.15) - ((indoorTemp - 32) * (5/9) + 273.15);
  float P = 16.07;
  int time = (m * c * dT) / P;
}

float convertFtoK(float F) {
  float time = (F - 32) * (5/9) + 273.15;
  return time;
}