#include <Arduino.h>
#include "Heater.h"

#define OFF false
#define ON true

int _heaterPin;    // output pin that controls the heater
int _fanTogglePin_heater; // output pin that toggles heater fan on/off
int _fanPwmPin_heater;    // output pin that controls fan speed


Heater::Heater(int heaterPin, int fanTogglePin_heater) {
  pinMode(heaterPin, OUTPUT);
  _heaterPin = heaterPin;

  pinMode(fanTogglePin_heater, OUTPUT);
  _fanTogglePin_heater = fanTogglePin_heater;
}

Heater::Heater(int heaterPin, int fanTogglePin_heater, int fanPwmPin_heater) {
  pinMode(heaterPin, OUTPUT);
  _heaterPin = heaterPin;

  pinMode(fanTogglePin_heater, OUTPUT);
  _fanTogglePin_heater = fanTogglePin_heater;

  pinMode(fanPwmPin_heater, OUTPUT);
  _fanPwmPin_heater = fanPwmPin_heater;
}

void Heater::toggleHeater(bool onOff) {
  digitalWrite(_heaterPin, onOff);
}

void Heater::toggleHeaterFan(bool onOff) {
  digitalWrite(_fanTogglePin_heater, onOff);
}

void Heater::changeFanSpeed(int percent) {
  if(percent > 100) { percent = 100; }
  if(percent < 0)   { percent = 0;   }
  int val = map(percent, 0, 100, 0, 255);
  analogWrite(_fanPwmPin_heater, val);
}