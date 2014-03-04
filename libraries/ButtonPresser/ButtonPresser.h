#ifndef BUTTON_PRESSER_H
#define BUTTON_PRESSER_H

#include "Arduino.h"
#include <Servo.h>

enum PresserPosition {
    REST,
    WITHDRAW,
    PRESS
};

class ButtonPresser
{
  public:
    ButtonPresser(int pin, Servo myservo);
    void Press();
    void Rest();
    void Withdraw();
    PresserPosition GetPosition();
  private:
    int _pin;
    Servo _myservo;
    PresserPosition position;
};

#endif