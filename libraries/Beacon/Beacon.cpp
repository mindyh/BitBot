#include "Arduino.h"
#include "Beacon.h"

Beacon::Beacon(int pin, int threshold) {
  _pin = pin;
  _threshold = threshold;
}

bool Beacon::IsFacingBeacon() {
    return (maxVal - minVal > _threshold);
}

void Beacon::Clear() {
  minVal = 500;
  maxVal = 500;
}

void Beacon::Update() {
  int currVal = analogRead(_pin);
  if(currVal <= minVal)
      minVal = currVal;
  if (currVal >= maxVal)
      maxVal = currVal;
}