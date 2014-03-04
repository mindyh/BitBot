#ifndef LINE_SENSORS_H
#define LINE_SENSORS_H

#include "Arduino.h"

class LineSensors
{
  public:
    LineSensors(int centerPin, int frontPin, int threshold);
    bool IsCenterOverLine();
    bool IsFrontOverLine();
    bool IsAlignedWithLine();
  private:
    int _front;
    int _center;
    int _threshold;
};

#endif