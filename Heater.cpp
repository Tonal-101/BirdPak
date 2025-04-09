#include <Arduino.h>
#include "Heater.h"

#define OFF true
#define ON false

int _heaterPin;    // output pin that controls the heater
int _fanTogglePin; // output pin that toggles heater fan on/off
int _fanPwmPin;    // output pin that controls fan speed


Heater::Heater(int heaterPin, int fanTogglePin) {
  pinMode(heaterPin, OUTPUT);
  _heaterPin = heaterPin;

  pinMode(fanTogglePin, OUTPUT);
  _fanPin = fanTogglePin;
}

Heater::Heater(int heaterPin, int fanTogglePin, int fanPwmPin) {
  pinMode(heaterPin, OUTPUT);
  _heaterPin = heaterPin;

  pinMode(fanTogglePin, OUTPUT);
  _fanPin = fanTogglePin;

  pinMode(fanPwmPin, OUTPUT);
  _fanPwmPin = fanPwmPin;
}

void Heater::toggleHeater(bool onOff) {
  digitalWrite(_heaterPin, onOff);
}

void Heater::toggleFan(bool onOff) {
  digitalWrite(_fanTogglePin, onOff);
}

void Heater::changeFanSpeed(int percent) {
  if(percent > 100) { percent = 100; }
  if(percent < 0)   { percent = 0;   }
  int val = map(percent, 0, 100, 0, 255);
  analogWrite(_fanPwmPin, val);
}