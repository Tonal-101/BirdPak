#include <Arduino.h>
#include "Speaker.h"

// Music notes & frequencies
    float _A0  = 27.50;
    float _Bb0 = 29.14;
    float _B0  = 30.87;

    float _C1  = 32.70;
    float _Db1 = 34.65;
    float _D1  = 36.71;
    float _Eb1 = 38.89;
    float _E1  = 41.20;
    float _F1  = 43.65;
    float _Gb1 = 46.25;
    float _G1  = 49.00;
    float _Ab1 = 51.91;
    float _A1  = 55.00;
    float _Bb1 = 58.27;
    float _B1  = 61.74;

    float _C2  = 65.41;
    float _Db2 = 69.30;
    float _D2  = 73.42;
    float _Eb2 = 77.78;
    float _E2  = 82.41;
    float _F2  = 87.31;
    float _Gb2 = 92.50;
    float _G2  = 98.00;
    float _Ab2 = 103.83;
    float _A2  = 110.00;
    float _Bb2 = 116.54;
    float _B2  = 123.47;

    float _C3  = 130.81;
    float _Db3 = 138.59;
    float _D3  = 146.83;
    float _Eb3 = 155.56;
    float _E3  = 164.81;
    float _F3  = 174.61;
    float _Gb3 = 185.00;
    float _G3  = 196.00;
    float _Ab3 = 207.65;
    float _A3  = 220.00;
    float _Bb3 = 233.08;
    float _B3  = 246.94;

    float _C4  = 261.63;  // Middle C
    float _Db4 = 277.18;
    float _D4  = 293.66;
    float _Eb4 = 311.13;
    float _E4  = 329.63;
    float _F4  = 349.23;
    float _Gb4 = 369.99;
    float _G4  = 392.00;
    float _Ab4 = 415.30;
    float _A4  = 440.00;  // Standard tuning
    float _Bb4 = 466.16;
    float _B4  = 493.88;

    float _C5  = 523.25;
    float _Db5 = 554.37;
    float _D5  = 587.33;
    float _Eb5 = 622.25;
    float _E5  = 659.25;
    float _F5  = 698.46;
    float _Gb5 = 739.99;
    float _G5  = 783.99;
    float _Ab5 = 830.61;
    float _A5  = 880.00;
    float _Bb5 = 932.33;
    float _B5  = 987.77;

    float _C6 = 1046.50;
    float _Db6 = 1108.73;
    float _D6 = 1174.66;
    float _Eb6 = 1244.51;
    float _E6 = 1318.51;
    float _F6 = 1396.91;
    float _Gb6 = 1479.98;
    float _G6 = 1567.98;
    float _Ab6 = 1661.22;
    float _A6 = 1760.00;
    float _Bb6 = 1864.66;
    float _B6 = 1975.53;

    float _C7  = 2093.00;
    float _Db7 = 2217.46;
    float _D7  = 2349.32;
    float _Eb7 = 2489.02;
    float _E7  = 2637.02;
    float _F7  = 2793.83;
    float _Gb7 = 2959.96;
    float _G7  = 3135.96;
    float _Ab7 = 3322.44;
    float _A7  = 3520.00;
    float _Bb7 = 3729.31;
    float _B7  = 3951.07;

    float _C8  = 4186.01;

int _speakerPin;

Speaker::Speaker(int speakerPin) {
  pinMode(speakerPin, OUTPUT);
  _speakerPin = speakerPin;
}

void Speaker::notif_tempReached() {
  // tone(_speakerPin, _E6);
  // delay(70);
  // noTone(_speakerPin);
  // delay(50);

  // tone(_speakerPin, _E6);
  // delay(70);
  // noTone(_speakerPin);
  // delay(50);
  
  tone(_speakerPin, _E4);
  delay(70);
  noTone(_speakerPin);
  delay(50);
  
  tone(_speakerPin, _B5);
  delay(200);
  noTone(_speakerPin);
}

void Speaker::notif_warning() {
  tone(_speakerPin, _C4);
  delay(200);
  noTone(_speakerPin);
  delay(200);
}

void Speaker::notif_monitorMode() {
  tone(_speakerPin, _Bb6);
  delay(100);
  noTone(_speakerPin);

  tone(_speakerPin, _Ab6);
  delay(100);
  noTone(_speakerPin);

  tone(_speakerPin, _Gb6);
  delay(100);
  noTone(_speakerPin);

  tone(_speakerPin, _Db6);
  delay(150);
  noTone(_speakerPin);
  delay(50);
}