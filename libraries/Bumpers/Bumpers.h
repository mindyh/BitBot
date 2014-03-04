#ifndef BUMPERS_H
#define BUMPERS_H

#include "Arduino.h"

class Bumpers
{
  public:
    Bumpers(int fl, int bl, int fr, int br);
    bool IsFrontLeftPressed();
    bool IsBackLeftPressed();
    bool IsFrontRightPressed();
    bool IsBackRightPressed();
    bool IsBackPressed();
    bool IsFrontPressed();
  private:
    int _fl;
    int _bl;
    int _fr;
    int _br;
};

#endif