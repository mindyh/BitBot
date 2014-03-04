#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include "Arduino.h"
#include "Drivetrain.h"
class Drivetrain
{
public:
    Drivetrain(int dir1_pin, int dir2_pin, int enbl1_pin, int enbl2_pin);
    void GoForward(int val);
    void GoBackward(int val);
    void SpinLeft(int val);
    void SpinRight(int val);
    void TurnRight(int val);
    void TurnLeft(int val);
    void Stop();
private:
  int _dir1_pin;
  int _dir2_pin;
  int _enbl1_pin;
  int _enbl2_pin;  
};

#endif