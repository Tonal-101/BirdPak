#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>

class Display {
  public:
    Display();
    void redBtnPushed();
    void blueBtnPushed();

    void updateDisplay_btnRed();
    void updateDisplay_btnBlue();

    void updateDisplay_currentTemp();
    void updateDisplay_outdoorTemp();
    void updateDisplay_targetTemp();
    void updateDisplay_timeToTemp();

    void alarm(String errorCode);

  private:
    void _initializeDisplay();
    int _ttHeader_Size;
    int _ttHeader_Height;
    int _ttValue_Size;
    int _ttValue_Height;
    int _tt_Height;
    int _ctHeader_Size;
    int _ctHeader_Height;
    int _ctHeader_Y;
    int _ctValue_Size;
    int _ctValue_Height;
    int _ctValue_Y;
    int _ct_Height;
    int _otHeader_Size;
    int _otHeader_Height;
    int _otHeader_Y;
    int _otValue_Size;
    int _otValue_Height;
    int _otValue_Y;
}