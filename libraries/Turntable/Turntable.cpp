#include "Arduino.h"
#include "Turntable.h"

Turntable::Turntable( int enbl, int dir)
{
  _dir = dir;
  _enbl = enbl;

  lastDir = CW;

  pinMode(_dir, OUTPUT);
  pinMode(_enbl, OUTPUT);
  //pinMode(_bumper, INPUT);
  Stop();
}

/*-----Turntable Functions -----*/
void Turntable::TurnCW(int val) {
    digitalWrite(_dir, LOW);
    analogWrite(_enbl, val);

    lastDir = CW;
}

void Turntable::TurnCCW(int val) {
    digitalWrite(_dir, HIGH);
    analogWrite(_enbl, val);

    lastDir = CCW;
}

TurntableDir Turntable::GetLastDir() {
  return lastDir;
}

void Turntable::Stop() {
    analogWrite(_enbl, 0);
}

bool Turntable::IsBumperPressed() {

}
    
int Turntable::GetCurrentSector(){
    
}
