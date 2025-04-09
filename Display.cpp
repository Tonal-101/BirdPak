#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>

#include "Display.h"
#include "Speaker.h"

// Colors ~~~~~~~~~~~~~~~~~
#define RED         0x9003
#define BLACK       0x0000
#define BLUE        0x015b
#define LIGHT_BLUE  0x07df
#define DARK_BLUE   0x037F
#define DARK_RED    0xD004
// ~~~~~~~~~~~~~~~~~~~~~~~~

// Speaker ~~~~~~~~~~~~~~~~
Speaker speaker(3); // speaker connected to digital pin 3
// ~~~~~~~~~~~~~~~~~~~~~~~~

// Display Calibration ~~~~
  // This is calibration data 
  // for the raw touch data to
  // the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define TS_MIN_PRESSURE 200

Adafruit_TSC2007 ts;

// The display also uses
// hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// ~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~ DISPLAY ORGANIZATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~[*] DISPLAY ORG \/
#define DISPLAY_HEIGHT 320
#define DISPLAY_WIDTH  240
#define MARGIN_TOP     10
#define MARGIN_LEFT    10
#define MARGIN_WIDE    50
#define MARGIN_MED     25

int _ttHeader_Size   = 2; // tt -> target temp
int _ttHeader_Height = _ttHeader_Size * 7;

int _ttValue_Size    = 6;
int _ttValue_Height  = _ttValue_Size * 7;

int _tt_Height = _ttHeader_Height + _ttValue_Height;


int _ctHeader_Size   = 2; // ct -> current temp
int _ctHeader_Height = _ctHeader_Size * 7;
int _ctHeader_Y      = MARGIN_TOP + _tt_Height + MARGIN_WIDE;

int _ctValue_Size    = 3;
int _ctValue_Height  = _ctValue_Size * 7;
int _ctValue_Y       = MARGIN_TOP + _tt_Height + MARGIN_WIDE + _ctHeader_Height + MARGIN_TOP;

int _ct_Height = _ctHeader_Height + MARGIN_TOP + _ctValue_Height;


int _otHeader_Size   = 2; // ot -> outdoor temp
int _otHeader_Height = _otHeader_Size * 7;
int _otHeader_Y      = MARGIN_TOP + _tt_Height + MARGIN_WIDE + _ct_Height + MARGIN_MED;

int _otValue_Size    = 3;
int _otValue_Height  = _otValue_Size * 7;
int _otValue_Y       = MARGIN_TOP + _tt_Height + MARGIN_WIDE + _ct_Height + MARGIN_MED + _otHeader_Height + MARGIN_TOP;
//~~~~ END DISPLAY ORGANIZATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Display::Display() {
  tft.begin();
  _initializeDisplay();
}

void Display::redBtnPushed() {
  // Swaps colors on click:
  // Turns blacks to dark red, whites to blacks, and swaps the button border color w/ the btn color

  // Red Button
  tft.fillRect(173, 0, 70, 160, 0xD004); // red button border
  tft.fillRect(177, 4, 59, 152, 0x9003); // red button 0x9003

  // Plus sign
  tft.fillRect(188, 73, 39, 14, 0x0000); // white border; plus sign horizontal part
  tft.fillRect(200, 61, 14, 39, 0x0000); // white border; plus sign vertical part
  tft.fillRect(190, 75, 35, 10, 0x6000); // plus sign horizontal part
  tft.fillRect(202, 63, 10, 35, 0x6000); // plus sign vertical part
}

void Display::blueBtnPushed() {
  // Swaps btn color for btn border color, makes minus-sign border black, and makes minus-sign dark blue

  // Blue Button
  tft.fillRect(173, 160, 70, 160, 0x037F); // blue button border
  tft.fillRect(177, 164, 59, 152, 0x015b); // blue button
  
  // minus sign
  tft.fillRect(188, 234, 39, 14, 0x0000); // white border; minus sign
  tft.fillRect(190, 236, 35, 10, 0x00ad); // minus sign
}

void Display::updateDisplay_btnRed() {
  // Red Button
  tft.fillRect(173, 0, 70, 160, 0x9003); // red button border
  tft.fillRect(177, 4, 59, 152, 0xD004); // red button
  tft.fillRect(188, 73, 39, 14, 0xfe5a); // white border; plus sign
  tft.fillRect(200, 61, 14, 39, 0xfe5a); // white border; plus sign
  tft.fillRect(190, 75, 35, 10, 0x0000); // plus sign horizontal part
  tft.fillRect(202, 63, 10, 35, 0x0000); // plus sign vertical part
}

void Display::updateDisplay_btnBlue() {
  // Blue Button
  tft.fillRect(173, 160, 70, 160, 0x015b); // blue button border
  tft.fillRect(177, 164, 59, 152, 0x037F); // blue button
  // Minus sign
  tft.fillRect(188, 234, 39, 14, 0xb7df); // white border; minus sign
  tft.fillRect(190, 236, 35, 10, 0x0000); // minus sign
}

void Display::updateDisplay_indoorTemp(int indoorTemp) {
  tft.fillRect(20, 125, 8*6*3, 3*7, 0x0000); // Erases on-screen current temp value
  
  // Current Temp
  tft.setCursor(20, 125);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print(String(indoorTemp));
  tft.write(0xF7);
  tft.print("F");
}

void Display::updateDisplay_outdoorTemp(int outdoorTemp) {
  tft.fillRect(20, 186, 8*6*3, 3*7, 0x0000); // Erases on-screen outdoor temp value
  
  // Outdoor Temp
  tft.setCursor(20, 186);
  tft.setTextSize(3);
  tft.setTextColor(0xFFFF);
  tft.print(String(outdoorTemp));
  tft.write(0xF7);
  tft.print("F");
}

void Display::updateDisplay_targetTemp(int targetTemp) {
  tft.fillRect(20, 36, 152, 6*7, 0x0000);    // Erases on-screen target temp value

  // Target Temp
  tft.setCursor(20, 36);
  tft.setTextSize(6);
  tft.setTextColor(0xFFFF);
  tft.print(String(targetTemp));
  tft.write(0xF7); //degree char
  tft.print("F");
}

void Display::updateDisplay_timeToTemp(int timeToTemp) {
  tft.fillRect(10, 240, 160, 14, 0x0000);

  tft.setCursor(10, 220);
  tft.setTextSize(2);
  tft.setTextColor(0xD004);
  tft.print("Time to Temp:");

  tft.setCursor(10, 240);
  tft.setTextColor(0xFFFF);
  tft.print(String(timeToTemp) + "sec");
}

void Display::alarm(String errorCode) {
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

  int loopCounter = 0;
  while(loopCounter < 8) { // plays alarm 8 times; main class can then call this alarm() method again if temps are still not safe
    tft.fillRect(111, 113, 18, 57, RED); // Exclamation point inside triangle
    tft.fillRect(111, 177, 18, 18, RED); // Exclamation point inside triangle

    tft.setCursor(52, 234);
    tft.setTextSize(4);
    tft.setTextColor(BLACK);
    tft.print("DANGER"); // 36px by 7px
    speaker.notif_warning();
    delay(300);

    tft.fillRect(111, 113, 18, 57, BLACK); // Exclamation point inside triangle
    tft.fillRect(111, 177, 18, 18, BLACK); // Exclamation point inside triangle

    tft.setCursor(52, 234);
    tft.setTextSize(4);
    tft.setTextColor(RED);
    tft.print("DANGER"); // 36px by 7px
    speaker.notif_warning();
    delay(400);

    loopCounter++;
  }
  _initializeDisplay(); // restarts display after loop ends
}

void Display::_initializeDisplay() {
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