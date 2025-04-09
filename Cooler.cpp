#include <Arduino.h>
#include "Cooler.h"

#define OFF true
#define ON false

int _coolerPin;    // output pin that controls the cooler
int _fanTogglePin; // output pin that toggles fan on/off
int _fanPwmPin;    // output pin that controls fan speed


Cooler::Cooler(int coolerPin, int fanTogglePin) {
  pinMode(coolerPin, OUTPUT);
  _coolerPin = CoolerPin;

  pinMode(fanPin, OUTPUT);
  _fanTogglePin = fanPin;
}

Cooler::Cooler(int coolerPin, int fanTogglePin, int fanPwmPin) {
  pinMode(coolerPin, OUTPUT);
  _coolerPin = CoolerPin;

  pinMode(fanTogglePin, OUTPUT);
  _fanTogglePin = fanPin;

  pinMode(fanPwmPin, OUTPUT);
  _fanPwmPin = fanPwmPin;
}

void Cooler::toggleCooler(bool onOff) {
  digitalWrite(_coolerPin, onOff);
}

void Cooler::toggleFan(bool onOff) {
  digitalWrite(_fanPin, onOff);
}

void Cooler::changeFanSpeed(int percent) {
  if(percent > 100) { percent = 100; }
  if(percent < 0)   { percent = 0;   }
  int val = map(percent, 0, 100, 0, 255);
  analogWrite(_fanPwmPin, val);
}