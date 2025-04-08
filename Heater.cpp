#include <Arduino.h>
#include "Heater.h"

#define OFF true
#define ON false

int _heaterPin; // output pin that controls the heater
int _fanPin;    // output pin that controls heater fan


Heater::Heater(int heaterPin, int fanPin) {
  pinMode(heaterPin, OUTPUT);
  _heaterPin = heaterPin;

  pinMode(fanPin, OUTPUT);
  _fanPin = fanPin;
}


void Heater::toggleHeater(bool onOff) {
  digitalWrite(_heaterPin, onOff);
}

void Heater::toggleFan(bool onOff) {
  digitalWrite(_fanPin, onOff);
}