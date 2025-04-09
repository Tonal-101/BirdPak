#include <Arduino.h>
#include "Cooler.h"

#define OFF true
#define ON false

int _coolerPin;    // output pin that controls the cooler
int _fanTogglePin_cooler; // output pin that toggles fan on/off
int _fanPwmPin_cooler;    // output pin that controls fan speed


Cooler::Cooler(int coolerPin, int fanTogglePin_cooler) {
  pinMode(coolerPin, OUTPUT);
  _coolerPin = coolerPin;

  pinMode(fanTogglePin_cooler, OUTPUT);
  _fanTogglePin_cooler = fanTogglePin_cooler;
}

Cooler::Cooler(int coolerPin, int fanTogglePin_cooler, int fanPwmPin_cooler) {
  pinMode(coolerPin, OUTPUT);
  _coolerPin = coolerPin;

  pinMode(fanTogglePin_cooler, OUTPUT);
  _fanTogglePin_cooler = fanTogglePin_cooler;

  pinMode(fanPwmPin_cooler, OUTPUT);
  _fanPwmPin_cooler = fanPwmPin_cooler;
}

void Cooler::toggleCooler(bool onOff) {
  digitalWrite(_coolerPin, onOff);
}

void Cooler::toggleFan(bool onOff) {
  digitalWrite(_fanTogglePin_cooler, onOff);
}

void Cooler::changeFanSpeed(int percent) {
  if(percent > 100) { percent = 100; }
  if(percent < 0)   { percent = 0;   }
  int val = map(percent, 0, 100, 0, 255);
  analogWrite(_fanPwmPin_cooler, val);
}