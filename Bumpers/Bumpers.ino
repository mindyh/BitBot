/*
 * Bumper sensors
 * ---------------
 * Mindy Huang
 */

  /*---------------- Module Defines ---------------------------*/
 #define BUMPER_BR_PIN    9
 #define BUMPER_BL_PIN    10
 #define BUMPER_FL_PIN    11
 #define BUMPER_FR_PIN    12

 #define FRONT  0
 #define BACK   1

 
/*--------Module Functions -------------*/
bool isBumperPressed(int pinNum) {
    return (digitalRead(pinNum) == HIGH);
}

bool isAlignedWithWall(int side) {
    switch(side) {
      case FRONT: 
        return (digitalRead(BUMPER_FR_PIN) == HIGH) &&
                (digitalRead(BUMPER_FL_PIN) == HIGH);
      case BACK:
        return (digitalRead(BUMPER_BR_PIN) == HIGH) &&
                (digitalRead(BUMPER_BL_PIN) == HIGH);
    }
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(BUMPER_BL_PIN, INPUT);
  pinMode(BUMPER_BR_PIN, INPUT);
  pinMode(BUMPER_FL_PIN, INPUT);
  pinMode(BUMPER_FR_PIN, INPUT);
}


void loop() {
  if(isBumperPressed(BUMPER_FR_PIN)) 
    Serial.println("front right");
  if(isBumperPressed(BUMPER_FL_PIN)) 
    Serial.println("front left");

  if(isBumperPressed(BUMPER_BL_PIN)) 
    Serial.println("back left");
  if(isBumperPressed(BUMPER_BR_PIN)) 
    Serial.println("back right");
    
  if(isAlignedWithWall(FRONT))
    Serial.println("Front aligned");
     
  if(isAlignedWithWall(BACK))
    Serial.println("back aligned"); 
}
 

 
 

