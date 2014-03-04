/*
 * Light sensors
 * ---------------
 * Mindy Huang
 */

  /*---------------- Module Defines ---------------------------*/
 #define LIGHT_PIN_FRONT A0
 #define LIGHT_PIN_CENTER A1
 
 #define THRESHOLD_VAL 200 
 
 
/*--------Module Functions -------------*/
bool isOverLine(int val) {
    return (val < THRESHOLD_VAL);
}

bool isAlignedWithLine() {
    return isOverLine(analogRead(LIGHT_PIN_FRONT)) && 
        isOverLine(analogRead(LIGHT_PIN_CENTER));
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(LIGHT_PIN_FRONT, INPUT);
  pinMode(LIGHT_PIN_CENTER, INPUT);
}


void loop() {
  if(isAlignedWithLine()) 
    Serial.println("aligned");
    
   
  
  if(isOverLine(analogRead(LIGHT_PIN_FRONT))) {
    Serial.println("FRONT over line");
  }
  
  if(isOverLine(analogRead(LIGHT_PIN_CENTER))) {
        Serial.println("CENTER over line");
  }
    
  Serial.print("FRONT: ");
  Serial.println(analogRead(LIGHT_PIN_FRONT));
  Serial.print("CENTER: ");
  Serial.println(analogRead(LIGHT_PIN_CENTER));
  delay(500);  
}
 

 
 
