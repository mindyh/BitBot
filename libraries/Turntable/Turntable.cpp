#include "Arduino.h"
#include "Turntable.h"

Turntable::Turntable( int enbl, int dir)
{
  _dir = dir;
  _enbl = enbl;

  pinMode(_dir, OUTPUT);
  pinMode(_enbl, OUTPUT);
  //pinMode(_bumper, INPUT);
  Stop();
}

/*-----Turntable Functions -----*/
void Turntable::TurnCW(int val) {
    digitalWrite(_dir, LOW);
    analogWrite(_enbl, val);
}

void Turntable::TurnCCW(int val) {
    digitalWrite(_dir, HIGH);
    analogWrite(_enbl, val);
}

void Turntable::Stop() {
    analogWrite(_enbl, 0);
}

bool Turntable::IsBumperPressed() {

}
    
int Turntable::GetCurrentSector(){
    
}
