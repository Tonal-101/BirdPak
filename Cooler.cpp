#include <Arduino.h>
#include "Cooler.h"

#define OFF true
#define ON false

int _coolerPin; // output pin that controls the cooler
int _fanPin;    // output pin that controls cooler fan


Cooler::Cooler(int coolerPin, int fanPin) {
  pinMode(coolerPin, OUTPUT);
  _coolerPin = CoolerPin;

  pinMode(fanPin, OUTPUT);
  _fanPin = fanPin;
}

void Cooler::toggleCooler(bool onOff) {
  digitalWrite(_coolerPin, onOff);
}

void Cooler::toggleFan(bool onOff) {
  digitalWrite(_fanPin, onOff);
}

void Cooler::changeFanSpeed(int rpm)