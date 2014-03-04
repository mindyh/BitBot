#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "Arduino.h"

class Turntable
{
  public:
    Turntable(int enbl, int dir);
    void TurnCW(int val);
    void TurnCCW(int val);
    void Stop();
    void BumperPressed();
    int GetCurrentPosition();
    //bool IsAlignedWithWall(int side);
  private:
    int _enbl;
    int _dir;
    int _bumper;
    int currSector;
};

#endif