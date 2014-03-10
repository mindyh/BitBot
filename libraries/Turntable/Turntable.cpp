#include "Arduino.h"
#include "Turntable.h"

Turntable::Turntable( int enbl, int dir, int clicker)
{
  _dir = dir;
  _enbl = enbl;
  _clicker = clicker;

  lastDir = CW;

  pinMode(_dir, OUTPUT);
  pinMode(_enbl, OUTPUT);
  pinMode(_clicker, INPUT);
  Stop();
}

/*-----Turntable Functions -----*/
void Turntable::TurnCW(int pwm) {
  digitalWrite(_dir, LOW);
  analogWrite(_enbl, pwm);

  lastDir = CW;
}

void Turntable::TurnCCW(int pwm) {
  digitalWrite(_dir, HIGH);
  analogWrite(_enbl, pwm);

  lastDir = CCW;
}

void Turntable::TurnCW(int pwm, int turns) {
  turnsToGo = turns;
  TurnCW(pwm);
}

void Turntable::TurnCCW(int pwm, int turns) {
  turnsToGo = turns;
  TurnCCW(pwm);
}

TurntableDir Turntable::GetLastDir() {
  return lastDir;
}

void Turntable::Stop() {
  analogWrite(_enbl, 0);
}

bool Turntable::IsClickerPressed() {
  return digitalRead(_clicker) == HIGH;
}
    
void Turntable::Update() {
  static bool clickerState = IsClickerPressed();
  static const bool initialState = clickerState;

  int currState;
  if ((currState = IsClickerPressed()) != clickerState) {
    clickerState = currState;

    if (clickerState == initialState) { 
      turnsToGo--;
    }

    if (turnsToGo == 0) {
      Stop();
    }
  }

}