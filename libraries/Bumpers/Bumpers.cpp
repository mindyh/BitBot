#include "Arduino.h"
#include "Bumpers.h"

Bumpers::Bumpers(int fl, 
                int bl,
                int fr,
                int br)
{
  _fr = fr;
  _bl = bl;
  _fl = fl;
  _br = br;  

  pinMode(_fr, INPUT); 
  pinMode(_bl, INPUT); 
  pinMode(_fl, INPUT);  
  pinMode(_br, INPUT); 
}

bool Bumpers::IsFrontLeftBumperPressed() {
    return (digitalRead(_fl) == HIGH);
}

bool Bumpers::IsBackLeftBumperPressed() {
    return (digitalRead(_bl) == HIGH);
}

bool Bumpers::IsFrontRightBumperPressed() {
    return (digitalRead(_fr) == HIGH);
}

bool Bumpers::IsBackRightBumperPressed() {
    return (digitalRead(_br) == HIGH);
}

bool Bumpers::IsAlignedWithWall(int side) {
    switch(side) {
      case FRONT: 
        return (IsFrontRightBumperPressed() &&
                IsFrontLeftBumperPressed());
      case BACK:
        return (IsBackRightBumperPressed() &&
                IsBackLeftBumperPressed());
    }
}