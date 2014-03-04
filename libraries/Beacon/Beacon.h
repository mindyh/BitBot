#ifndef BEACON_H
#define BEACON_H

#include "Arduino.h"

class Beacon
{
  public:
    Beacon(int pin, int threshold);
    bool IsFacingBeacon();
    void Clear(); 
    void Update();
  private:
    int minVal;
    int maxVal;
    int _threshold;
    int _pin;
};

#endif