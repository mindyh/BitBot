#include "Arduino.h"
#include "LineSensors.h"

LineSensors::LineSensors(int centerPin, 
                        int frontPin, 
                        int threshold) {
    _center = centerPin;
    _front = frontPin;
    _threshold = threshold;
}


bool LineSensors::IsCenterOverLine() {
    return (analogRead(_center) < _threshold);
}

bool LineSensors::IsFrontOverLine() {
    return (analogRead(_front) < _threshold);
}

bool LineSensors::IsAlignedWithLine() {
    return IsCenterOverLine() && IsFrontOverLine();
}