/*
* BotCoin.ino
* ---------------
* Mindy Huang
*/

#include <Timers.h>
#include "BotCoin.h"
#include <Drivetrain.h>
#include <Bumpers.h>
#include <Beacon.h>
#include <LineSensors.h>
#include <Turntable.h>
#include <ButtonPresser.h>
#include <Servo.h>

/*----- Module Variables ----*/
static Direction currentDirection;

Servo myservo;
Bumpers bumpers(BUMPER_FL_PIN, BUMPER_BL_PIN, BUMPER_FR_PIN, BUMPER_BR_PIN);
Beacon serverBeacon(BEACON_SERVER_PIN, BEACON_THRESHOLD_VAL);
Beacon exchangeBeacon(BEACON_EXCHANGE_PIN, BEACON_THRESHOLD_VAL);
LineSensors lineSensors(LIGHT_PIN_CENTER, LIGHT_PIN_FRONT, LIGHT_THRESHOLD_VAL);
Drivetrain drivetrain(MOTOR_1_DIR_PIN, MOTOR_2_DIR_PIN, MOTOR_1_ENABLE_PIN, MOTOR_2_ENABLE_PIN);
Turntable turntable(TURNTABLE_ENABLE_PIN, TURNTABLE_DIR_PIN);
ButtonPresser presser(BUTTON_PRESSER_PIN, myservo);


/*---- Main Program ---*/
void setup(){
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    pinMode(HEARTBEAT_LED, OUTPUT);
    myservo.attach(BUTTON_PRESSER_PIN);

    // init variables
    digitalWrite(HEARTBEAT_LED, HIGH);
    serverBeacon.Clear();
    exchangeBeacon.Clear();
    drivetrain.Stop();
    presser.Press();

    // Start timers
    TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC / 2);   // to know we're alive
    TMRArd_InitTimer(ENDGAME_TIMER, 120 * ONE_SEC);   // Bot to shut off automatically in 2 minutes
    TMRArd_InitTimer(CLEAR_BEACON_TIMER, BEACON_CLEAR_PERIOD); // we have 50Hz sampling rate on beacons
    TMRArd_InitTimer(BUTTON_PRESS_TIMER, BUTTON_PRESS_INTERVAL);
    TMRArd_SetTimer(BRAKING_TIMER, BRAKING_INTERVAL);
}

void loop()
{
    static State currState = WAITING_TO_START;
    static State returnToState;

    // Always do
    if (TMRArd_IsTimerExpired(HEARTBEAT_TIMER))
    {
         TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC);
         digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED));
    }

    exchangeBeacon.Update();
    serverBeacon.Update();

    // if(TMRArd_IsTimerExpired(ENDGAME_TIMER)) {
    //     currState = WAITING_TO_END;
    //     turntable.Stop();
    //     drivetrain.Stop();
    // }

    if (TMRArd_IsTimerExpired(CLEAR_BEACON_TIMER))
    {
         serverBeacon.Clear();
         exchangeBeacon.Clear();
         TMRArd_InitTimer(CLEAR_BEACON_TIMER, BEACON_CLEAR_PERIOD);
    }

    // State dependent
    switch (currState)
    {
    case WAITING_TO_START:
        if (bumpers.IsFrontPressed())
        {
            currState = SEEKING_SERVER;
            drivetrain.SpinLeft(SPIN_RATE);
        }
        break;
    case SEEKING_SERVER:
        if (serverBeacon.IsFacingBeacon())
        {
            presser.Rest();
            drivetrain.GoForward(DRIVE_RATE);
            TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME);
            currState = TRAVELLING_TO_SERVER;
        }
        break;
    
    case TRAVELLING_TO_SERVER:
        if(lineSensors.IsFrontOverLine() && bumpers.IsFrontPressed()) {
            drivetrain.Stop(); 
            currState = BUTTON_PRESSING;
        } else if(lineSensors.IsCenterOverLine()) {
            drivetrain.Stop();
            currState = TRACKING_LINE;
//            drivetrain.SpinLeft(SPIN_RATE); 
//            currState = SEEKING_SERVER;
        } else if(bumpers.IsFrontPressed()) {
              drivetrain.GoBackward(DRIVE_RATE);
              currState = BACKING_UP;
              returnToState = SEEKING_SERVER;
              TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME/2);
        } else if(TMRArd_IsTimerExpired(TRAVELLING_TIMER) && !serverBeacon.IsFacingBeacon()) {
            drivetrain.SpinLeft(SPIN_RATE); 
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = SEEKING_SERVER;
        } 
        break;
        
    case TRACKING_LINE:
      if(bumpers.IsFrontPressed()) {
        currState = BUTTON_PRESSING;
        drivetrain.Stop();
      } else {
        TrackLine();
      }
      break;
      
//    case BRAKING:
//       if(TMRArd_IsTimerExpired(BRAKING_TIMER)) {
//         
//       }
//       break;
        
    case BACKING_UP:
        if(TMRArd_IsTimerExpired(TRAVELLING_TIMER)) {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = returnToState;
            switch(returnToState) {
                case TRAVELLING_TO_SERVER:
                  drivetrain.GoForward(DRIVE_RATE);
                  break;
                case SEEKING_SERVER:
                  drivetrain.SpinLeft(SPIN_RATE);
                  break;                  
                default: break;
            }
        }
        break;
//    case ALIGNING_WITH_EXCHANGE:
//        if (exchangeBeacon.IsFacingBeacon()) 
//        {
//           drivetrain.SpinLeft(170); 
//           currState = ALIGNING_WITH_SERVER;
//        }
//        break;
        
    case ALIGNING_WITH_WALL:
        static boolean first = true;
        static Bumper targetBumper;
        if(first) {
          if(bumpers.IsFrontPressed())
              targetBumper = bumpers.IsFrontLeftPressed() ? FL : FR;
          else if(bumpers.IsBackPressed())
              targetBumper = bumpers.IsBackLeftPressed() ? BL : BR;
              
          first = false;
        }
        
        if(targetBumper == FL || targetBumper == BL)
            drivetrain.TurnRight(150);
        else 
          drivetrain.TurnLeft(150);
          
        if((bumpers.IsFrontLeftPressed() && targetBumper == FL) ||
            (bumpers.IsFrontRightPressed() && targetBumper == FR) ||
            (bumpers.IsBackLeftPressed() && targetBumper == BL) ||
            (bumpers.IsBackRightPressed() && targetBumper == BR)) {
              drivetrain.GoBackward(DRIVE_RATE);
              currState = BACKING_UP;
              returnToState = SEEKING_SERVER;
              TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME/2);
            }
        
        break;
        
    case BUTTON_PRESSING: 
          static int numPresses = 0;
            if (numPresses == 5)
            {
                presser.Rest();
                numPresses = 0;
               // currState = WAITING_TO_END;
               // drivetrain.Stop();
               // currState = SEEKING_EXCHANGE;
               currState = TRAVELLING_TO_EXCHANGE;
                drivetrain.GoBackward(DRIVE_RATE);
            } else {
                PressButton(&numPresses);
            }
            break;
            
    case SEEKING_EXCHANGE:
        if (exchangeBeacon.IsFacingBeacon())
        {
            currState = TRAVELLING_TO_EXCHANGE;
            drivetrain.GoForward(170);
        }
        break;
    case TRAVELLING_TO_EXCHANGE:
        if (bumpers.IsBackPressed())
        {
            currState = DISPENSING;
            drivetrain.Stop();
            turntable.TurnCW(200);
            TMRArd_InitTimer(DISPENSER_TIMER, 4*ONE_SEC);
        }
        break;
    case DISPENSING:
        //static int numBumperPresses = 0;
        if (TMRArd_IsTimerExpired(DISPENSER_TIMER))
        {
            turntable.Stop();
            currState = SEEKING_SERVER;
        }
        break;
    case WAITING_TO_END:
        // do nothing
        break;
    default: break;
    }
}

/*--------Module Functions -------------*/
enum bumperState {
    GOING,
    STOPPED
};

enum beaconState {
    SEEKING_BEACON,
    SEEKING_LINE2,
    FOUND
};


void TestBeacon(Beacon b) {
    static beaconState bstate = SEEKING_BEACON;
    switch(bstate) {
        case SEEKING_BEACON:
            if (b.IsFacingBeacon())
            {
                Serial.println("facing beacon");
                drivetrain.Stop();
                bstate = FOUND;
//                bstate = SEEKING_LINE2;
//                drivetrain.GoForward(170);
            }
            else
                drivetrain.SpinLeft(170);
            break;
//        case SEEKING_LINE2:
//          if(lineSensors.IsCenterOverLine()) {
//            drivetrain.Stop();
//            bstate = FOUND;
//          }
//          break;
        default: break;
    }
}

void TestBumper() {
    static bumperState bstate = GOING;
    switch(bstate) {
        case GOING:
            if(bumpers.IsFrontPressed()){
                bstate = STOPPED;
                drivetrain.Stop();
            }
            else
                drivetrain.GoForward(200);
            break;
        default: break;      
    }
}


void PressButton(int* numPresses) {
    if (TMRArd_IsTimerExpired(BUTTON_PRESS_TIMER))
    {
        if (presser.GetPosition() == PRESS) 
            presser.Withdraw();
        else {
            presser.Press();
            (*numPresses)++;
        }        
        
        TMRArd_InitTimer(BUTTON_PRESS_TIMER, BUTTON_PRESS_INTERVAL);
    }
}

void TrackLine()
{
    static LineTrackingState currState = SEEKING_LINE;
    static LineTurningDir dir = LEFT;

    switch (currState)
    {
    case SEEKING_LINE:
        Serial.println("SEEKING");
        if (lineSensors.IsCenterOverLine())
        {
            if (dir == LEFT)
            {
                drivetrain.SpinLeft(SPIN_RATE);
                dir = RIGHT;
            }
            else
            {                   
                drivetrain.SpinRight(SPIN_RATE);
                dir = LEFT; 
            }
            currState = ALIGNING;
        }
        break;
    case ALIGNING:
        Serial.println("ALIGN");
        if (lineSensors.IsFrontOverLine() && lineSensors.IsCenterOverLine())
        {
            drivetrain.GoForward(DRIVE_RATE);
            currState = FOLLOWING_LINE;
        }
       if (!lineSensors.IsCenterOverLine())
        {
            drivetrain.GoForward(DRIVE_RATE);
            currState = SEEKING_LINE;
        }
        break;
    case FOLLOWING_LINE:
        Serial.println("following");
        if (!lineSensors.IsCenterOverLine() || !lineSensors.IsFrontOverLine())
        {
            drivetrain.GoForward(DRIVE_RATE);
            currState = SEEKING_LINE;
        }
        break;
    }
}

unsigned char TestForKey(void)
{
    unsigned char KeyEventOccurred;

    KeyEventOccurred = Serial.available();
    return KeyEventOccurred;
}

void RespondToKey(void)
{
    unsigned char theKey = Serial.read();

    Serial.print(theKey);
    Serial.print(", ASCII=");
    Serial.println(theKey, HEX);

    //  switch (theKey) {
    //    case '0':
    //        Stop();
    //        Serial.println("stop");
    //        break;
    //    case '1':
    //        goForward(100);
    //        Serial.println("goForward");
    //        break;
    //    case '2':
    //        goBackward(100);
    //        Serial.println("goBackward");
    //        break;
    //    case '3':
    //        SpinLeft(100);
    //        Serial.println("SpinLeft");
    //        break;
    //    case '4':
    //        SpinRight(100);
    //        Serial.println("SpinRight");
    //        break;
    //    default: break;
    //  }
}
