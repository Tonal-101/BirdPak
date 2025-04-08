#ifndef Speaker_h
#define Speaker_h

#include <Arduino.h>

class Speaker {
  public:
    Speaker(int speakerPin);
    void notif_tempReached();
    void notif_warning();
    void notif_monitorMode();

  private:
    int _speakerPin;

    // Music notes & frequencies
    float A0_  = 27.50;
    float Bb0_ = 29.14;
    float B0_  = 30.87;

    float C1_  = 32.70;
    float Db1_ = 34.65;
    float D1_  = 36.71;
    float Eb1_ = 38.89;
    float E1_  = 41.20;
    float F1_  = 43.65;
    float Gb1_ = 46.25;
    float G1_  = 49.00;
    float Ab1_ = 51.91;
    float A1_  = 55.00;
    float Bb1_ = 58.27;
    float B1_  = 61.74;

    float C2_  = 65.41;
    float Db2_ = 69.30;
    float D2_  = 73.42;
    float Eb2_ = 77.78;
    float E2_  = 82.41;
    float F2_  = 87.31;
    float Gb2_ = 92.50;
    float G2_  = 98.00;
    float Ab2_ = 103.83;
    float A2_  = 110.00;
    float Bb2_ = 116.54;
    float B2_  = 123.47;

    float C3_  = 130.81;
    float Db3_ = 138.59;
    float D3_  = 146.83;
    float Eb3_ = 155.56;
    float E3_  = 164.81;
    float F3_  = 174.61;
    float Gb3_ = 185.00;
    float G3_  = 196.00;
    float Ab3_ = 207.65;
    float A3_  = 220.00;
    float Bb3_ = 233.08;
    float B3_  = 246.94;

    float C4_  = 261.63;  // Middle C
    float Db4_ = 277.18;
    float D4_  = 293.66;
    float Eb4_ = 311.13;
    float E4_  = 329.63;
    float F4_  = 349.23;
    float Gb4_ = 369.99;
    float G4_  = 392.00;
    float Ab4_ = 415.30;
    float A4_  = 440.00;  // Standard tuning
    float Bb4_ = 466.16;
    float B4_  = 493.88;

    float C5_  = 523.25;
    float Db5_ = 554.37;
    float D5_  = 587.33;
    float Eb5_ = 622.25;
    float E5_  = 659.25;
    float F5_  = 698.46;
    float Gb5_ = 739.99;
    float G5_  = 783.99;
    float Ab5_ = 830.61;
    float A5_  = 880.00;
    float Bb5_ = 932.33;
    float B5_  = 987.77;

    float C6_  = 1046.50;
    float Db6_ = 1108.73;
    float D6_  = 1174.66;
    float Eb6_ = 1244.51;
    float E6_  = 1318.51;
    float F6_  = 1396.91;
    float Gb6_ = 1479.98;
    float G6_  = 1567.98;
    float Ab6_ = 1661.22;
    float A6_  = 1760.00;
    float Bb6_ = 1864.66;
    float B6_  = 1975.53;

    float C7_  = 2093.00;
    float Db7_ = 2217.46;
    float D7_  = 2349.32;
    float Eb7_ = 2489.02;
    float E7_  = 2637.02;
    float F7_  = 2793.83;
    float Gb7_ = 2959.96;
    float G7_  = 3135.96;
    float Ab7_ = 3322.44;
    float A7_  = 3520.00;
    float Bb7_ = 3729.31;
    float B7_  = 3951.07;

    float C8_  = 4186.01;
};

#endif