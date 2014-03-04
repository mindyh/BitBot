#ifndef BUMPERS_H
#define BUMPERS_H

#include "Arduino.h"

class Bumpers
{
  public:
    Bumpers(int fl, int bl, int fr, int br);
    bool IsFrontLeftBumperPressed();
    bool IsBackLeftBumperPressed();
    bool IsFrontRightBumperPressed();
    bool IsBackRightBumperPressed();
    bool IsAlignedWithWall(int side);
  private:
    int _fl;
    int _bl;
    int _fr;
    int _br;
};

#endif