#include "Heater.h"
#include "Cooler.h"
#include "Speaker.h"
#include "Display.h"
#include <OneWire.h>
#include <DallasTemperature.h>


// Variables for Button Bounding Boxes ~~~~~~~~~~
  // For temperature increment button
  int redBtn_x1 = 160;
  int redBtn_y1 = 0;
  int redBtn_x2 = 240;
  int redBtn_y2 = 160;

  // For temperature decrement button
  int blueBtn_x1 = 160;
  int blueBtn_y1 = 160;
  int blueBtn_x2 = 240;
  int blueBtn_y2 = 320;

  // For monitor mode button
  int monitorModeBtn_x1 = 10;
  int monitorModeBtn_y1 = 225;
  int monitorModeBtn_x2 = 165;
  int monitorModeBtn_y2 = 275;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Variables for Temperature Sensors ~~~~~~~~~~~~
  // Sensor Addresses
  uint8_t sensorAddress_indoor[8]  = {0x28, 0xD0, 0xA7, 0x55, 0x00, 0x00, 0x00, 0xE1}; // 64-bit address of DS18B20 temp sensor
  uint8_t sensorAddress_outdoor[8] = {0x28, 0x4A, 0xAC, 0x59, 0x00, 0x00, 0x00, 0xFD};
  uint8_t sensorAddress_heater[8];
  uint8_t sensorAddress_cooler[8];

  int tempThresholdF_high = 100; // in *F
  int tempThresholdF_low  = 50;  // in *F

  // Data wire is plugged into port 4 on the Arduino
  #define ONE_WIRE_BUS 4
  // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
  OneWire oneWire(ONE_WIRE_BUS);

  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors(&oneWire);

  int numberOfDevices; // Number of temperature devices found
  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
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

  int highTempThreshold = 95; // *F
  int lowTempThreshold  = 45; // *F

  int  timeToTemp; // estimated time until enclosure reaches the target temperature
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Monitor Mode Variables ~~~~~~~~~~~~~~~~~~~~~~~
  bool isMonitorModeOn;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Display display;
  Heater heater(heaterPin, heaterFanPin);
  Cooler cooler(coolerPin, coolerFanPin);
  Speaker speaker(speakerPin);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.begin(115200);
  sensors.begin();
  initializeSensors();
  display.initializeDisplay();
}

void loop() {
  sensors.requestTemperatures();

  // Shift old temp readings to -Temp_prev variables
  targetTemp_prev = targetTemp;
  indoorTemp_prev = indoorTemp;
  outdoorTemp_prev = outdoorTemp;

  // Get new temp readings
  indoorTemp = sensors.getTempF(sensorAddress_indoor);
  outdoorTemp = sensors.getTempF(sensorAddress_outdoor);
  heaterTemp = sensors.getTempF(sensorAddress_heater);
  coolerTemp = sensors.getTempF(sensorAddress_cooler);

  if(indoorTemp >= highTempThreshold) { 
    display.alarm("Hi Temps"); 
  }
  if(indoorTemp <= lowTempThreshold) { 
    display.alarm("Lo Temps");
  }

  checkBtnPresses();
  checkReachedTargetTemp(); // Turns on/off cooler and heater as needed

  if(indoorTemp != indoorTemp_prev)   { display.updateDisplay_indoorTemp(indoorTemp);   }
  if(outdoorTemp != outdoorTemp_prev) { display.updateDisplay_outdoorTemp(outdoorTemp); }
  if(targetTemp != targetTemp_prev)   { display.updateDisplay_targetTemp(targetTemp);   }
}

void initializeSensors() {
  // Sensor setup
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

  // setHighAlarmTemp(HIGH_TEMP_ALARM);
  // setLowAlarmTemp(LOW_TEMP_ALARM);
}

void checkBtnPresses() {
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

  if(display.isBtnPushed(monitorModeBtn_x1, monitorModeBtn_y1, monitorModeBtn_x2, monitorModeBtn_y2)) {
    if(!isMonitorModeOn) {
      // If monitor mode is off, turn on monitor mode and keep button pushed
      isMonitorModeOn = true;
      display.monitorModeBtnPushed();
      speaker.notif_monitorMode();
    } else if(isMonitorModeOn) {
      // If monitor mode is on, turn off monitor mode and release button push
      isMonitorModeOn = false;
      display.updateDisplay_btnMonitorMode();
      speaker.notif_tempReached();
    }
  }
}

void checkReachedTargetTemp() {
  if(indoorTemp == targetTemp) { 
    if(playNotif) { 
      speaker.notif_tempReached(); // notify user when target temp is reached
      playNotif = false; // so notification sound won't constantly play
    } 
    toggleHeatModule(OFF);
    toggleCoolerModule(OFF);
  } else if(indoorTemp < targetTemp) { // If too cold, turn on heater + heater fan
    // If inside is 3*F colder than target, turn on heater, turn off cooler

    // Why 1*F exactly? Because 5*F seemed like too much, and I sure as hell
    // don't want the heater and cooler flipping on and off for split seconds
    toggleHeatModule(ON);
    toggleCoolerModule(OFF);
    playNotif = true;
  } else if(indoorTemp > targetTemp) { // If too hot, turn on cooler + cooler fan
    // if inside is 3*F hotter than target, turn off heater, turn on cooler
    toggleCoolerModule(ON);
    toggleHeatModule(OFF);
    playNotif = true;
  }
  
  if(isMonitorModeOn) {
    // if Monitor Mode is on, turn off all external devices
    // Heater fan is left on for ventilation
    heater.toggleHeater(OFF);
    cooler.toggleCooler(OFF);
    cooler.toggleCoolerFan(OFF);
    heater.toggleHeaterFan(ON); // Leave on for ventilation
  }
}

void toggleHeatModule(bool onOff) {
  if(onOff) {
    heater.toggleHeater(ON);
    heater.toggleHeaterFan(ON);
  } else {
    heater.toggleHeater(OFF);
    heater.toggleHeaterFan(OFF);
  }
}

void toggleCoolerModule(bool onOff) {
  if(onOff) {
    cooler.toggleCooler(ON);
    cooler.toggleCoolerFan(ON);
  } else {
    cooler.toggleCooler(OFF);
    cooler.toggleCoolerFan(OFF);
  }
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