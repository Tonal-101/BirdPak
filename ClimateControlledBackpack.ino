
// DISPLAY SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// TODO: 
// Include various coordinates for where the different readings will be displayed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// TEMPERATURE/SENSOR SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /* DS18B20 Temp Sensor Libraries */
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

  // Code from: https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices;
DeviceAddress tempDeviceAddress;

  /* Sensor/Button Pins */
int btnTempUp            = 2;
int btnTempDown          = 3;

  /* Sensor Device Addresses */
int enclosureTempSensor1 = 0;
int enclosureTempSensor2 = 1;
int outsideTempSensor    = 2;

  /* Temperatures */
int targetTemp           = 0;
int currentEnclosureTemp = 0;
int currentOutsideTemp   = 0;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// DATA VALIDATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool prevState_btnTempUp   = false;
bool prevState_btnTempDown = false;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// BATTERY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float voltage_BatteryEmpty = 0;
float voltage_BatteryFull  = 0;
float voltage_Range  = voltage_BatteryFull - voltage_BatteryEmpty;
float currentVoltage       = 0;
int currentBatteryPercentage = 0;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// HEATER ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int heaterPin = 4;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.begin(115200);

  // DISPLAY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // TEMP SENSORS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Code from: https://randomnerdtutorials.com/guide-for-ds18b20-temperature-sensor-with-arduino/
  sensors.begin();
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
      printAddress(tempDeviceAddress);
      Serial.println();
		} else {
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
		}
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // BUTTONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pinMode(btnTempUp, INPUT);
  pinMode(btnTempDown, INPUT);
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

void loop() {
  // Update values & fetch new sensor data
  updateBatteryPercentage();
  currentOutsideTemp = getOutsideTemp();
  currentEnclosureTemp = getEnclosureTemp();

  if(isBtnPressed(btnTempUp))   { targetTemp++; }
  if(isBtnPressed(btnTempDown)) { targetTemp--; }

  toggleHeater(reachedTargetTemp());

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // display new target temp
  // display updated outside temp
  // display updated inside temp
  // display updated battery percentage
  // refresh display
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

}

bool isBtnPressed(btnPin) {
  // This function distinguishes between pressing a button and holding down a button
  if(btnPin == btnTempUp) {
    if(digitalRead(btnPin) && !prevState_btnTempUp) { prevState_btnTempUp = true;  return true;  }
    else /****************************************/ { prevState_btnTempUp = false; return false; }
  } 
  
  if (btnPin == btnTempDown) {
    if(digitalRead(btnPin) && !prevState_btnTempDown) { prevState_btnTempDown = true;  return true;  }
    else /******************************************/ { prevState_btnTempDown = false; return false; }
  }
}

void updateBatteryPercentage() {
  currentVoltage = readVcc();
  // 1% of battery capacity is (voltageWhenFull - voltageWhenEmpty) / 100
  // So if we divide the number of volts above empty by that 1%, then we will find the percentage
  currentBatteryPercentage = (currentVoltage - voltage_BatteryEmpty) / (voltage_Range / 100);
  return currentBatteryPercentage;
}

long readVcc() {
  // Code from: https://forum.arduino.cc/t/can-an-arduino-measure-the-voltage-of-its-own-power-source/669954/5
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

int getTempSensorData_F(sensorIndex) {
  // Gets temp in F, rounds it, then returns that rounded value
  return round(sensors.getTempFByIndex(sensorIndex));
}

int getOutsideTemp() {
  return getTempSensorData_F(outsideTempSensor);
}

int getEnclosureTemp() {
  // Returns the average of the two enclosure temperature sensors
  return round((getTempSensorData_F(enclosureTempSensor1) + getTempSensorData_F(enclosureTempSensor2)) / 2);
}

bool reachedTargetTemp() {
  if(targetTemp == getEnclosureTemp()) { return true; }
  else { return false; }
}

void toggleHeater(bool onOff) {
  if(onOff) { digitalWrite(heaterPin, HIGH); } 
  else if(!onOff) { digitalWrite(heaterPin, LOW); }
}