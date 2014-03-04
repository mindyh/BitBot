/*
 * Motors
 * ---------------
 * Mindy Huang
 */
 
 #include <Timers.h>
 
 /*---------------- Module Defines ---------------------------*/
#define MOTOR_1_DIR_PIN         4
#define MOTOR_1_ENABLE_PIN      5
#define MOTOR_2_DIR_PIN         7
#define MOTOR_2_ENABLE_PIN      6

#define HEARTBEAT_LED  13
#define ONE_SEC   1000
#define HEARTBEAT_TIMER    1

enum Direction {
  FORWARD,
  BACKWARD,
  SPIN_LEFT,
  SPIN_RIGHT
};
 
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

void goForward(int val) {
    digitalWrite(MOTOR_1_DIR_PIN, HIGH);
    digitalWrite(MOTOR_2_DIR_PIN, LOW);

    analogWrite(MOTOR_1_ENABLE_PIN, val);
    analogWrite(MOTOR_2_ENABLE_PIN, val);
}

void goBackward(int val){
    digitalWrite(MOTOR_1_DIR_PIN, LOW);
    digitalWrite(MOTOR_2_DIR_PIN, HIGH);

    analogWrite(MOTOR_1_ENABLE_PIN, val);
    analogWrite(MOTOR_2_ENABLE_PIN, val);
}

void SpinLeft(int val) {
    digitalWrite(MOTOR_1_DIR_PIN, HIGH);
    digitalWrite(MOTOR_2_DIR_PIN, HIGH);

    analogWrite(MOTOR_1_ENABLE_PIN, val);
    analogWrite(MOTOR_2_ENABLE_PIN, val);
}

void SpinRight(int val) {
    digitalWrite(MOTOR_1_DIR_PIN, LOW);
    digitalWrite(MOTOR_2_DIR_PIN, LOW);

    analogWrite(MOTOR_1_ENABLE_PIN, val);
    analogWrite(MOTOR_2_ENABLE_PIN, val);
}

void Stop(){
    analogWrite(MOTOR_1_ENABLE_PIN, 0);
    analogWrite(MOTOR_2_ENABLE_PIN, 0);
}
 
 /*---- Main Program ---*/
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(MOTOR_1_DIR_PIN, OUTPUT);
  pinMode(MOTOR_1_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_2_DIR_PIN, OUTPUT);
  pinMode(MOTOR_2_ENABLE_PIN, OUTPUT);  
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


 

 
 
