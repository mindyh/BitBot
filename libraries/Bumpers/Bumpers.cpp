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

bool Bumpers::IsFrontLeftPressed() {
    return (digitalRead(_fl) == HIGH);
}

bool Bumpers::IsBackLeftPressed() {
    return (digitalRead(_bl) == HIGH);
}

bool Bumpers::IsFrontRightPressed() {
    return (digitalRead(_fr) == HIGH);
}

bool Bumpers::IsBackRightPressed() {
    return (digitalRead(_br) == HIGH);
}

bool Bumpers::IsFrontPressed() {
    return (IsFrontLeftPressed() || IsFrontRightPressed());
}

bool Bumpers::IsBackPressed() {
    return (IsBackLeftPressed() || IsBackRightPressed());
}

// bool Bumpers::IsAlignedWithWall(int side) {
//     switch(side) {
//       case FRONT: 
//         return (IsFrontRightBumperPressed() &&
//                 IsFrontLeftBumperPressed());
//       case BACK:
//         return (IsBackRightBumperPressed() &&
//                 IsBackLeftBumperPressed());
//     }
// }