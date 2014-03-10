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
    Turntable(int enbl, int dir, int clicker);
    void TurnCW(int pwm);
    void TurnCCW(int pwm);
    void TurnCW(int pwm, int turns);    // turns is the number of QUARTER turns
    void TurnCCW(int pwm, int turns);   // turns is the number of QUARTER turns
    void Stop();
    bool IsClickerPressed();
    void Update();
    TurntableDir GetLastDir();
  private:
    int _enbl;
    int _dir;
    int _clicker;
    int turnsToGo;
    TurntableDir lastDir;
};

#endif