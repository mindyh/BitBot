/*
 * ButtonPress
 * ---------------
 * Mindy Huang
 */
 
 
#include <Servo.h> 
#include <Timers.h>
 
 /*---------------- Module Defines ---------------------------*/
#define SERVO_PIN         8

#define HEARTBEAT_LED  13
#define ONE_SEC   1000
#define HEARTBEAT_TIMER    1


 /*--------Module Functions -------------*/
unsigned char TestForKey(void) {
  unsigned char KeyEventOccurred;
  
  KeyEventOccurred = Serial.available();
  return KeyEventOccurred;
}

void RespondToKey(void) {
  unsigned char theKey = Serial.read();
  
  Serial.print(theKey);
  Serial.print(", ASCII=");
  Serial.println(theKey,HEX);
  
  switch (theKey) {
    case '0': 
        Stop(); 
        Serial.println("stop");
        break;
    case '1': 
        goForward(150);
        Serial.println("goForward");
        break;
    case '2':
        goBackward(150);
        Serial.println("goBackward");
        break;
    case '3':
        SpinLeft(150);
        Serial.println("SpinLeft");
        break;
    case '4':
        SpinRight(150);
        Serial.println("SpinRight");
        break;
    default: break;
  }
}

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 

 /*---- Main Program ---*/
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  myservo.attach(SERVO_PIN);
 
   for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
    {                                  // in steps of 1 degree 
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
    for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
    {                                
      myservo.write(pos);              // tell servo to go to position in variable 'pos' 
      delay(15);                       // waits 15ms for the servo to reach the position 
    } 
  
  pinMode(HEARTBEAT_LED, OUTPUT); 
  digitalWrite(HEARTBEAT_LED, HIGH);
  TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC/2);
  
  Stop();
}

void loop() {
  static Direction dir = FORWARD;
  
  if(TestForKey()) RespondToKey();
  if(TMRArd_IsTimerExpired(HEARTBEAT_TIMER)) {
    TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC);
    digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED));
  }

  // switch (dir) {
  //   case FORWARD:
  //       if(TestForKey()) RespondToKey();
            
  //       break;
  //   case BACKWARD:
  //       break;
  //   case SPIN_LEFT:
  //       break;
  //   case SPIN_RIGHT:
  //       break;
  //   case default: break;
  //  }
}


 

 
 
