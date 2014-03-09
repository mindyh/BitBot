/*
 * Beacons
 * ---------------
 * Mindy Huang
 */
 
 #include <Timers.h>
 
 /*---------------- Module Defines ---------------------------*/
#define BEACON_EXCHANGE_PIN    A4
#define BEACON_SERVER_PIN    A5

#define HEARTBEAT_LED  13
#define ONE_SEC   1000

#define HEARTBEAT_TIMER  1
#define BEACON_CLEAR_TIMER ONE_SEC/10

 /*--------Module Functions -------------*/
unsigned char TestForKey(void) {
  unsigned char KeyEventOccurred;
  
  KeyEventOccurred = Serial.available();
  return KeyEventOccurred;
}

bool isFacingServer(int min, int max) {
    return (max - min > 250);
}
 
 /*---- Main Program ---*/
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(BEACON_SERVER_PIN, INPUT);
  pinMode(BEACON_EXCHANGE_PIN, INPUT);
  pinMode(HEARTBEAT_LED, OUTPUT);

  digitalWrite(HEARTBEAT_LED, HIGH);
  TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC);
  TMRArd_InitTimer(BEACON_CLEAR_TIMER, ONE_SEC/10);
}

void loop() {
    static int minServerBeaconVal = 0;
    static int maxServerBeaconVal = 0;
    static int minExchangeBeaconVal = 0;
    static int maxExchangeBeaconVal = 0;

    // clear min and max beacon values intermittently
    if (TMRArd_IsTimerExpired(BEACON_CLEAR_TIMER)) {
        minServerBeaconVal = 500;
        maxServerBeaconVal = 500;
        minExchangeBeaconVal = 500;
        maxExchangeBeaconVal = 500;
        TMRArd_InitTimer(BEACON_CLEAR_TIMER, ONE_SEC);
       // Serial.println("clear");
    }

    int currExchangeBeaconVal = analogRead(BEACON_EXCHANGE_PIN);
    int currServerBeaconVal = analogRead(BEACON_SERVER_PIN);

    // update exchange beacon values
    if(currExchangeBeaconVal < minExchangeBeaconVal)
        minExchangeBeaconVal = currExchangeBeaconVal;
    else if (currExchangeBeaconVal > maxExchangeBeaconVal)
        maxExchangeBeaconVal = currExchangeBeaconVal;

    // update server beacon values
    if(currServerBeaconVal < minServerBeaconVal)
        minServerBeaconVal = currServerBeaconVal;
    else if (currServerBeaconVal > maxServerBeaconVal)
        maxServerBeaconVal = currServerBeaconVal;

//   Serial.println(maxServerBeaconVal - minServerBeaconVal);
////
//    if(isFacingServer(minServerBeaconVal, maxServerBeaconVal))
//        Serial.println("facing server");
    //Serial.println(currServerBeaconVal);

    if(isFacingServer(minExchangeBeaconVal, maxExchangeBeaconVal))
        Serial.println("facing exchange");

  if(TMRArd_IsTimerExpired(HEARTBEAT_TIMER)) {
    TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC);
    digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED));
  }
}


 

 
 
