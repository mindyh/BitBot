#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "Arduino.h"

enum TurntableDir {
    CW,
    CCW
};

class Turntable
{
  public:
    Turntable(int enbl, int dir);
    void TurnCW(int val);
    void TurnCCW(int val);
    void Stop();
    bool IsBumperPressed();
    int GetCurrentSector();
    TurntableDir GetLastDir();
  private:
    int _enbl;
    int _dir;
    int _bumper;
    int currSector;
    TurntableDir lastDir;
};

#endif