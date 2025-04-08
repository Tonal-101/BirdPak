#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// TODO:
// Add temp sensor to heating element
// Add functionality that turns off heating element when the element itself reaches a threshold temp
// Add functionality that keeps fan on even if heater is off
// Add GUI elements that indicate heater temp/status & fan status



//~~~~ FONTS & COLORS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] FONTS & COLORS \/
#define RED         0x9003
#define BLACK       0x0000
#define BLUE        0x015b
#define LIGHT_BLUE  0x07df
#define DARK_BLUE   0x037F
#define DARK_RED    0xD004
//~~~~ END FONTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~





//~~~~ GLOBAL TEMPERATURE VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] TEMP VARS \/
#define OFF true
#define ON false
int targetTemp = 71; // initial target temp (*F)

// environment sensors
int currentTemp; // current temp inside enclosure
int outdoorTemp;

// equipment sensors
int heater_Temp;
int cooler_Temp;

bool targetTempChanged;   // 'true' if current temperature is different from previous temperature
bool currentTempChanged;
bool outdoorTempChanged;
int timeToTemp; // estimated time until enclosure reaches the target temperature

uint8_t sensor_current[8] = {0x28, 0xD0, 0xA7, 0x55, 0x00, 0x00, 0x00, 0xE1}; // 64-bit address of DS18B20 temp sensor
uint8_t sensor_outdoor[8] = {0x28, 0x4A, 0xAC, 0x59, 0x00, 0x00, 0x00, 0xFD};
uint8_t sensor_heater[8];

int const HIGH_TEMP_ALARM = 90; // degrees F
int const LOW_TEMP_ALARM  = 50; // degrees F
//~~~~ END GLOBAL TEMPERATURE VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




//~~~~ OUTPUT PINS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] OUTPUT PINS \/
// temp sensor pin = 4;
int heaterPin    = 5; // output pin that controls ALL HEATERS
int fanPin       = 6; // output pin that controls heater fan
int coolerPin    = 7; // output pin that controls ALL COOLERS
int coolerFanPin = 11; // output pin that controls cooler fan (PWM capable)
int speakerPin   = A1; // output pin that controls the onboard passive buzzer
//~~~~ END OUTPUT PINS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




//~~~~ DISPLAY SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] DISPLAY SETUP \/
// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define TS_MIN_PRESSURE 200

Adafruit_TSC2007 ts;

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//~~~~ END DISPLAY SETUP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




//~~~~ TEMPERATURE SENSOR SET UP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] TEMP SENSOR SETUP \/
// Data wire is plugged into port 4 on the Arduino
#define ONE_WIRE_BUS 4
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
//~~~~ END TEMPERATURE SENSOR SET UP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




//~~~~ DISPLAY ORGANIZATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] DISPLAY ORG \/
#define DISPLAY_HEIGHT 320
#define DISPLAY_WIDTH  240
#define MARGIN_TOP     10
#define MARGIN_LEFT    10
#define MARGIN_WIDE    50
#define MARGIN_MED     25

int ttHeader_Size   = 2; // tt -> target temp
int ttHeader_Height = ttHeader_Size * 7;

int ttValue_Size    = 6;
int ttValue_Height  = ttValue_Size * 7;

int tt_Height = ttHeader_Height + ttValue_Height;


int ctHeader_Size   = 2; // ct -> current temp
int ctHeader_Height = ctHeader_Size * 7;
int ctHeader_Y      = MARGIN_TOP + tt_Height + MARGIN_WIDE;

int ctValue_Size    = 3;
int ctValue_Height  = ctValue_Size * 7;
int ctValue_Y       = MARGIN_TOP + tt_Height + MARGIN_WIDE + ctHeader_Height + MARGIN_TOP;

int ct_Height = ctHeader_Height + MARGIN_TOP + ctValue_Height;


int otHeader_Size   = 2; // ot -> outdoor temp
int otHeader_Height = otHeader_Size * 7;
int otHeader_Y      = MARGIN_TOP + tt_Height + MARGIN_WIDE + ct_Height + MARGIN_MED;

int otValue_Size    = 3;
int otValue_Height  = otValue_Size * 7;
int otValue_Y       = MARGIN_TOP + tt_Height + MARGIN_WIDE + ct_Height + MARGIN_MED + otHeader_Height + MARGIN_TOP;
//~~~~ END DISPLAY ORGANIZATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




void setup() {
  Serial.begin(115200);
  
  tft.begin();
  sensors.begin();

  initializeDisplay();
  initializeSensors();

  pinMode(heaterPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
}

void loop() {
  sensors.requestTemperatures(); // fetches temperature readings from DS18B20 temperature sensors
  updateAllTemps();   // Sets all temperature variables to the newly fetched values (DOES NOT AFFECT DISPLAY)
  updateTimeToTemp();
  displayTimeToTemp();

  
  toggleFan(true); // true = On, false = Off
  // if(heater1_Temp >= HIGH_TEMP_ALARM) { toggleFans(ON); }
  // else { toggleFans(OFF); }

  if(currentTemp >= targetTemp) {
    toggleHeater(OFF);
  } else if(currentTemp < targetTemp) {
    toggleHeater(ON);
  }

  if(currentTemp >= HIGH_TEMP_ALARM) { alarm("Hi Temps"); }
  if(currentTemp <= LOW_TEMP_ALARM)  { alarm("Lo Temps"); }

  checkBtnPresses();  // Checks if a button was pressed: if yes, update targetTemp & show pressed button graphic

  if(targetTempChanged)  { updateDisplayTemp_target();  }
  if(currentTempChanged) { updateDisplayTemp_current(); }
  if(outdoorTempChanged) { updateDisplayTemp_outdoor(); }
  targetTempChanged = false; // resets to false for next loop
  currentTempChanged = false;
  outdoorTempChanged = false;
}


void initializeDisplay() {
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  // Graphics setup
  tft.setRotation(180);
  tft.fillScreen(ILI9341_BLACK);

  //~~~~ INITIALIZE DISPLAY GRAPHICS LAYOUT ~~~~

  // BUTTON GRAPHICS
  // Red Button
  tft.fillRect(173, 0, 70, 160, 0x9003); // red button border
  tft.fillRect(177, 4, 59, 152, 0xD004); // red button
  // Blue Button
  tft.fillRect(173, 160, 70, 160, 0x015b); // blue button border
  tft.fillRect(177, 164, 59, 152, 0x037F); // blue button

  // Plus & minus signs
  tft.fillRect(188, 73, 39, 14, 0xfe5a); // white border; plus sign horizontal part
  tft.fillRect(200, 61, 14, 39, 0xfe5a); // white border; plus sign vertical part
  tft.fillRect(190, 75, 35, 10, 0x0000); // plus sign horizontal part
  tft.fillRect(202, 63, 10, 35, 0x0000); // plus sign vertical part

  tft.fillRect(188, 234, 39, 14, 0xb7df); // white border; minus sign
  tft.fillRect(190, 236, 35, 10, 0x0000); // minus sign


  // "Target Temp" Subheader
  tft.setCursor(10, 12);
  tft.setTextSize(2);
  tft.setTextColor(0xD004);
  tft.print("Target Temp");

  // Target Temp Value
  tft.setCursor(20, 36);
  tft.setTextSize(6);
  tft.setTextColor(0xFFFF);
  tft.print("--");
  tft.write(0xF7); //degree char
  tft.print("F");

  tft.fillRect(10, 100, 5, 115, 0x037f); // decorative bar: ||

  // "Current Temp" Subheader
  tft.setCursor(20, 105);
  tft.setTextSize(2);
  tft.setTextColor(0x07df);
  tft.print("Current Temp");

  // Current Temp Value
  tft.setCursor(20, 125);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print("--");
  tft.write(0xF7);
  tft.print("F");

  // "Outdoor Temp" Header
  tft.setCursor(20, 166);
  tft.setTextSize(2);
  tft.setTextColor(0x07df);
  tft.print("Outdoor Temp");

  // Outdoor Temp Value
  tft.setCursor(20, 186);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print("--");
  tft.write(0xF7);
  tft.print("F");
}

void redBtnPushed() {
  // Swap_s colors on click:
  // Turns_ blacks to dark red, whites to blacks, and swaps the button border color w/ the btn colo_r_

  // Red Button
  tft.fillRect(173, 0, 70, 160, 0xD004); // red button border
  tft.fillRect(177, 4, 59, 152, 0x9003); // red button 0x9003

  // Plus sign
  tft.fillRect(188, 73, 39, 14, 0x0000); // white border; plus sign horizontal part
  tft.fillRect(200, 61, 14, 39, 0x0000); // white border; plus sign vertical part
  tft.fillRect(190, 75, 35, 10, 0x6000); // plus sign horizontal part
  tft.fillRect(202, 63, 10, 35, 0x6000); // plus sign vertical part
}

void blueBtnPushed() {
  // Swaps btn color for btn border color, makes minus-sign border black, and makes minus-sign dark blue

  // Blue Button
  tft.fillRect(173, 160, 70, 160, 0x037F); // blue button border
  tft.fillRect(177, 164, 59, 152, 0x015b); // blue button
  
  // minus sign
  tft.fillRect(188, 234, 39, 14, 0x0000); // white border; minus sign
  tft.fillRect(190, 236, 35, 10, 0x00ad); // minus sign
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

void updateAllTemps() {
  updateCurrentTemp();
  updateOutdoorTemp();
  updateHeater1Temp();
}

void updateDisplayTemp_target() {
  tft.fillRect(20, 36, 152, 6*7, 0x0000);    // Erases on-screen target temp value

  // Target Temp
  tft.setCursor(20, 36);
  tft.setTextSize(6);
  tft.setTextColor(0xFFFF);
  tft.print(String(targetTemp));
  tft.write(0xF7); //degree char
  tft.print("F");
}

void updateDisplayTemp_current() {
  tft.fillRect(20, 125, 8*6*3, 3*7, 0x0000); // Erases on-screen current temp value
  
  // Current Temp
  tft.setCursor(20, 125);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print(String(currentTemp));
  tft.write(0xF7);
  tft.print("F");
}

void updateDisplayTemp_outdoor() {
  tft.fillRect(20, 186, 8*6*3, 3*7, 0x0000); // Erases on-screen outdoor temp value
  
  // Outdoor Temp
  tft.setCursor(20, 186);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print(String(outdoorTemp));
  tft.write(0xF7);
  tft.print("F");
}

void updateCurrentTemp() {
  sensors.requestTemperaturesByAddress(sensor_current);
  int prev_currentTemp = currentTemp;
  currentTemp = round(sensors.getTempF(sensor_current));

  if(prev_currentTemp != currentTemp) { currentTempChanged = true; }
}

int getCurrentTemp() {
  sensors.requestTemperaturesByAddress(sensor_current);
  currentTemp = round(sensors.getTempF(sensor_current));
  return currentTemp;
}

void updateOutdoorTemp() {
  int prev_outdoorTemp = outdoorTemp;
  outdoorTemp = round(sensors.getTempF(sensor_outdoor));

  if(prev_outdoorTemp != outdoorTemp) { outdoorTempChanged = true; }
}

void updateHeater1Temp() {
  heater_Temp = round(sensors.getTempF(sensor_heater));
}

void updateTimeToTemp() {
  float m = 0.0322;
  float c = 1005;
  int dT = ((targetTemp - 32) * (5/9) + 273.15) - ((currentTemp - 32) * (5/9) + 273.15);
  float P = 16.07;
  int time = (m * c * dT) / P;
}

void displayTimeToTemp() {
  tft.fillRect(10, 240, 160, 14, 0x0000);

  tft.setCursor(10, 220);
  tft.setTextSize(2);
  tft.setTextColor(0xD004);
  tft.print("Time to Temp:");

  tft.setCursor(10, 240);
  tft.setTextColor(0xFFFF);
  tft.print(String(timeToTemp) + "sec");
}

float convertFtoK(float F) {
  float time = (F - 32) * (5/9) + 273.15;
  return time;
}

void setFanRPM(int rpm) {
  if(rpm == 0) { digitalWrite(fanPin, OFF); }
  else { /* code to adjust fan speed */ }
}

void alarm(String errorCode) {
  // Error Codes:
  // H1 : Heaters too hot
  // H2 : Heaters not responding
  // T1 : Enclosure Temps too hot
  // T2 : Enclosure Temps too cold

  tft.fillScreen(RED); // turn display red
  tft.fillTriangle(120, 52, 30, 215, 210, 215, BLACK); // place black triangle upper center
  
  // error code text
  tft.setCursor(10, 275);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Error: " + errorCode);

  while(currentTemp >= HIGH_TEMP_ALARM || currentTemp <= LOW_TEMP_ALARM) { // keep playing alarm until temperature reaches OK level
    getCurrentTemp(); // fetch latest currentTemp value
    tft.fillRect(111, 113, 18, 57, RED); // Exclamation point inside triangle
    tft.fillRect(111, 177, 18, 18, RED); // Exclamation point inside triangle

    tft.setCursor(52, 234);
    tft.setTextSize(4);
    tft.setTextColor(BLACK);
    tft.print("DANGER"); // 36px by 7px
    notif_warning();
    delay(300);

    tft.fillRect(111, 113, 18, 57, BLACK); // Exclamation point inside triangle
    tft.fillRect(111, 177, 18, 18, BLACK); // Exclamation point inside triangle

    tft.setCursor(52, 234);
    tft.setTextSize(4);
    tft.setTextColor(RED);
    tft.print("DANGER"); // 36px by 7px
    notif_warning();
    delay(400);
  }

  initializeDisplay(); // restarts display after reaching nominal temps
}

void checkBtnPresses() {
  uint16_t x, y, z1, z2;
  if (ts.read_touch(&x, &y, &z1, &z2) && (z1 > TS_MIN_PRESSURE)) {

    Serial.print("Touch point: (");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.print(z1); Serial.print(" / ");
    Serial.print(z2); Serial.println(")");  
   
    // Scale from ~0->4000 to tft.width using the calibration #'s
    x = map(x, TS_MINX, TS_MAXX, 0, tft.width());
    y = map(y, TS_MINY, TS_MAXY, 0, tft.height());

    if(x > 160 && y < 160) {
      // Red button is pressed
      targetTemp++;
     targetTempChanged = true;
      isValidTemp();
      redBtnPushed();
      delay(50); // retain pressed look for 50ms
      resetBtn_Red(); // set button back to default look
    } else if(x > 160 && y > 160) {
      // B_lue button is pressed
      targetTemp--;
     targetTempChanged = true;
      isValidTemp();
      blueBtnPushed();
      delay(50);
      resetBtn_Blue();
    }
  }
}

void isValidTemp() {
  if(targetTemp < 0) { targetTemp = 0; targetTempChanged = false; }
  else if(targetTemp > 99) { targetTemp = 99; targetTempChanged = false; }
}

void resetBtn_Red() { // sets button graphics to their default look
  // Red Button
  tft.fillRect(173, 0, 70, 160, 0x9003); // red button border
  tft.fillRect(177, 4, 59, 152, 0xD004); // red button
  tft.fillRect(188, 73, 39, 14, 0xfe5a); // white border; plus sign
  tft.fillRect(200, 61, 14, 39, 0xfe5a); // white border; plus sign
  tft.fillRect(190, 75, 35, 10, 0x0000); // plus sign horizontal part
  tft.fillRect(202, 63, 10, 35, 0x0000); // plus sign vertical part

}

void resetBtn_Blue() {
// Blue Button
  tft.fillRect(173, 160, 70, 160, 0x015b); // blue button border
  tft.fillRect(177, 164, 59, 152, 0x037F); // blue button
  // Minus sign
  tft.fillRect(188, 234, 39, 14, 0xb7df); // white border; minus sign
  tft.fillRect(190, 236, 35, 10, 0x0000); // minus sign
}