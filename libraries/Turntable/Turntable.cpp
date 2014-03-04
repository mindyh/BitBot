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
void TurnCW(int val) {
    digitalWrite(_dir, LOW);
    analogWrite(TURNTABLE_ENABLE_PIN, val);
}

void TurnCCW(int val) {
    digitalWrite(_dir, HIGH);
    analogWrite(_enbl, val);
}

void Stop() {
    analogWrite(_enbl, 0);
}

void IsBumperPressed() {

}
    
int GetCurrentSector(){
    
}
