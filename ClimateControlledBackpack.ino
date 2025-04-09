#include "Heater.h"
#include "Cooler.h"
#include "Speaker.h"
#include "Temperature_Sensor.h"
#include "Display.h"


// Variables for Button Bounding Boxes ~~~~~~~~~~
  int redBtn_x1 = 160;
  int redBtn_y1 = 0;
  int redBtn_x2 = 240;
  int redBtn_y2 = 160;

  int blueBtn_x1 = 160;
  int blueBtn_y1 = 160;
  int blueBtn_x2 = 240;
  int blueBtn_y2 = 320;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Temperature Sensors ~~~~~~~~~~~~
  // Sensor Addresses
  uint8_t sensorAddress_indoor[8]  = {0x28, 0xD0, 0xA7, 0x55, 0x00, 0x00, 0x00, 0xE1}; // 64-bit address of DS18B20 temp sensor
  uint8_t sensorAddress_outdoor[8] = {0x28, 0x4A, 0xAC, 0x59, 0x00, 0x00, 0x00, 0xFD};
  uint8_t sensorAddress_heater[8];
  uint8_t sensorAddress_cooler[8];

  int tempThresholdF_high = 100; // in *F
  int tempThresholdF_low  = 50;  // in *F
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Heater ~~~~~~~~~~~~~~~~~~~~~~~~~
  int heaterPin = 5; // output pin that controls ALL HEATERS
  int heaterTemp;   // stores the temperature of the heating unit

  int heaterFanPin = 6;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Cooler ~~~~~~~~~~~~~~~~~~~~~~~~~
  int coolerPin = 7; // output pin that controls ALL COOLERS
  int coolerTemp;   // stores the temperature of the cooling unit

  int coolerFanPin = 11; // output pin that controls cooler fan (PWM capable)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Speaker ~~~~~~~~~~~~~~~~~~~~~~~~
  int speakerPin = 3; // output pin that controls the onboard passive buzzer
  bool playNotif = true;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// General Temperature Variables ~~~~~~~~~~~~~~~~
#define OFF false
#define ON true

int targetTemp = 71; // default target temp (*F)

int indoorTemp; // current temp inside enclosure
int outdoorTemp; // current temp outside enclosure

// These are used to determine if the display needs to be refreshed
int targetTemp_prev;
int indoorTemp_prev;
int outdoorTemp_prev;

bool targetTempChanged;   // 'true' if current temperature is different from previous temperature
bool indoorTempChanged;   // ^^^
bool outdoorTempChanged;  // ^^^

int highTempThreshold = 95; // *F
int lowTempThreshold  = 45; // *F

int  timeToTemp; // estimated time until enclosure reaches the target temperature
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// Objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Display display;
  Heater heater(heaterPin, heaterFanPin);
  Cooler cooler(coolerPin, coolerFanPin);
  Speaker speaker(speakerPin);

  Temperature_Sensor allSensors;
  Temperature_Sensor indoorSensor(sensorAddress_indoor, tempThresholdF_high, tempThresholdF_low);
  Temperature_Sensor outdoorSensor(sensorAddress_outdoor);
  Temperature_Sensor heaterSensor(sensorAddress_heater);
  Temperature_Sensor coolerSensor(sensorAddress_cooler);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.begin(115200);
  // pinMode(4, INPUT_PULLUP);
  display.initializeDisplay();
  allSensors.initializeSensors();
}

void loop() {
  // allSensors.requestTemperatures();

  // Shift old temp readings to -Temp_prev variables
  targetTemp_prev = targetTemp;
  indoorTemp_prev = indoorTemp;
  outdoorTemp_prev = outdoorTemp;

  // Get new temp readings
  indoorTemp = indoorSensor.getTempF();
  outdoorTemp = outdoorSensor.getTempF();
  heaterTemp = heaterSensor.getTempF();
  coolerTemp = coolerSensor.getTempF();

  if(indoorTemp >= 95) { 
    // display.alarm("Hi Temps"); 
  }
  if(indoorTemp <= 45) { 
    // display.alarm("Lo Temps");
  }

  // Check if buttons are pressed
  if(display.isBtnPushed(redBtn_x1, redBtn_y1, redBtn_x2, redBtn_y2)) {
    targetTemp++;
    // keeps within bounds; also the graphics break down if targetTemp is more than 2 digits
    if(targetTemp >= 100) { targetTemp = 99; }
    display.redBtnPushed();
    delay(50); // retain pushed-button-look for 50ms
    display.updateDisplay_btnRed();
  } else if(display.isBtnPushed(blueBtn_x1, blueBtn_y1, blueBtn_x2, blueBtn_y2)) {
    targetTemp--;
    // keeps above 0*F, because that seems like a logical lower bound
    if(targetTemp < 0) { targetTemp = 0; }
    display.blueBtnPushed();
    delay(50); // retain pushed-button-look for 50ms
    display.updateDisplay_btnBlue();
  }

  if(indoorTemp == targetTemp) { 
    if(playNotif) { 
      speaker.notif_tempReached(); // notify user when target temp is reached
      playNotif = false; // so notification sound won't constantly play
    } 
    heater.toggleHeater(OFF);
    cooler.toggleCooler(OFF);
  } else if(indoorTemp < targetTemp - 3) {
    // If inside is 3*F colder than target, turn on heater, turn off cooler
    // Why 3*F exactly? Because 5*F seemed like too much, and I sure as hell
    // don't want the heater and cooler flipping on and off for split seconds
    heater.toggleHeater(ON);
    heater.toggleHeaterFan(ON);

    cooler.toggleCooler(OFF);
    cooler.toggleCoolerFan(OFF);
    playNotif = true;
  } else if(indoorTemp > targetTemp + 3) {
    // if inside is 3*F hotter than target, turn off heater, turn on cooler
    cooler.toggleCooler(ON);
    cooler.toggleCoolerFan(ON);

    heater.toggleHeater(OFF);
    heater.toggleHeaterFan(OFF);
    playNotif = true;
  }

  if(indoorTemp != indoorTemp_prev)   { display.updateDisplay_indoorTemp(indoorTemp);   }
  if(outdoorTemp != outdoorTemp_prev) { display.updateDisplay_outdoorTemp(outdoorTemp); }
  if(targetTemp != targetTemp_prev)   { display.updateDisplay_targetTemp(targetTemp);   }
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