#include "Arduino.h"
#include "Drivetrain.h"

Drivetrain::Drivetrain( int dir1_pin, 
                        int dir2_pin,
                        int enbl1_pin,
                        int enbl2_pin)
{
  _dir1_pin = dir1_pin;
  _dir2_pin = dir2_pin;
  _enbl1_pin = enbl1_pin;
  _enbl2_pin = enbl2_pin;  

  pinMode(_dir1_pin, OUTPUT);
  pinMode(_dir2_pin, OUTPUT);
  pinMode(_enbl1_pin, OUTPUT);
  pinMode(_enbl2_pin, OUTPUT); 
  Stop();
}

/*------Drivetrain functions-----*/
void Drivetrain::GoForward(int val) {
    digitalWrite(_dir1_pin, HIGH);
    digitalWrite(_dir2_pin, LOW);

    analogWrite(_enbl1_pin, val);
    analogWrite(_enbl2_pin, val);
}

void Drivetrain::GoBackward(int val){
    digitalWrite(_dir1_pin, LOW);
    digitalWrite(_dir2_pin, HIGH);

    analogWrite(_enbl1_pin, val);
    analogWrite(_enbl2_pin, val);
}

void Drivetrain::SpinLeft(int val) {
    digitalWrite(_dir1_pin, HIGH);
    digitalWrite(_dir2_pin, HIGH);

    analogWrite(_enbl1_pin, val);
    analogWrite(_enbl2_pin, val);
}

void Drivetrain::SpinRight(int val) {
    digitalWrite(_dir1_pin, LOW);
    digitalWrite(_dir2_pin, LOW);

    analogWrite(_enbl1_pin, val);
    analogWrite(_enbl2_pin, val);
}

void Drivetrain::TurnRight(int val) {
    digitalWrite(_dir1_pin, LOW);

    analogWrite(_enbl1_pin, val);
    analogWrite(_enbl2_pin, 0);
}

void Drivetrain::TurnLeft(int val) {
    digitalWrite(_dir1_pin, HIGH);

    analogWrite(_enbl1_pin, 0);
    analogWrite(_enbl2_pin, val);
}

void Drivetrain::Stop() {
    analogWrite(_enbl1_pin, 0);
    analogWrite(_enbl2_pin, 0);
}
