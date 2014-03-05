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

Direction switchSpinDir = SPIN_LEFT;
static unsigned long time = 0;

/*---- Main Program ---*/
void setup()
{
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
    TMRArd_InitTimer(CLEAR_BEACON_TIMER, BEACON_CLEAR_PERIOD); // we have 20Hz sampling rate on beacons

    // clear timers to make sure they don't start expired
    TMRArd_ClearTimerExpired(BUTTON_PRESS_TIMER);
    TMRArd_ClearTimerExpired(BRAKING_TIMER);
    TMRArd_ClearTimerExpired(LINE_TRACKING_TIMER);
    TMRArd_ClearTimerExpired(ALIGNING_TIMER);
}

void loop()
{
    static State currState = WAITING_TO_START;
    //static State currState = BUTTON_PRESSING;
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
            SwitchSpin(true);
        }
        break;
    case SEEKING_SERVER:
        if (serverBeacon.IsFacingBeacon())
        {
            presser.Rest();
            Brake(SPIN);
            returnToState = TRAVELLING_TO_SERVER;
            currState = BRAKING;
        }
        break;

    case TRAVELLING_TO_SERVER:
        if (lineSensors.IsFrontOverLine() && bumpers.IsFrontPressed())
        {
            drivetrain.Stop();
            currState = BUTTON_PRESSING;
            TMRArd_InitTimer(BUTTON_PRESS_TIMER, BUTTON_PRESS_INTERVAL);
        }
        else if (lineSensors.IsCenterOverLine())
        {
            currState = BRAKING;
            returnToState = ALIGNING_WITH_SERVER;
            Brake(BACKWARD);
            //            drivetrain.SpinLeft(SPIN_RATE);
            //            currState = SEEKING_SERVER;
        }
        else if (bumpers.IsFrontPressed())
        {
            drivetrain.GoBackward(DRIVE_RATE);
            currState = BACKING_UP;
            returnToState = SEEKING_SERVER;
            TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME / 2);
        }
        else if (TMRArd_IsTimerExpired(TRAVELLING_TIMER) && !serverBeacon.IsFacingBeacon())
        {
            SwitchSpin(true);
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = SEEKING_SERVER;
        }
        break;
        
    case ALIGNING_WITH_SERVER: 
        if(serverBeacon.IsFacingBeacon()) {
          Brake(SPIN);
          currState = BRAKING;
          returnToState = GOING_TO_SERVER_WALL;
        }
        break;
        

//    case SEEKING_LINE:
//        if (bumpers.IsFrontPressed() && serverBeacon.IsFacingBeacon())
//        {
//            currState = ALIGNING_WITH_WALL;
//            turntable.TurnCW(200);
//            drivetrain.GoForward(SLOW_DRIVE_RATE);
////            TMRArd_StopTimer(LINE_TRACKING_TIMER);
//            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
//        } else if (lineSensors.IsCenterOverLine()) {
//            SwitchSpin(true);
//            currState = ALIGNING;
////            TMRArd_InitTimer(LINE_TRACKING_TIMER, LINE_TRACKING_TIME); 
//        } else if (TMRArd_IsTimerExpired(TRAVELLING_TIMER)) {
//            currState = SEEKING_SERVER;
//            SwitchSpin(true);
//            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
//        } 
//        else {
//            //drivetrain.GoBackward(DRIVE_RATE);
//            if(!TMRArd_IsTimerActive) 
//              TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME);
//        }
//        break;
//        
//    case ALIGNING:
//        if (bumpers.IsFrontPressed() && serverBeacon.IsFacingBeacon())
//        {
//            currState = ALIGNING_WITH_WALL;
//            turntable.TurnCW(200);
//            drivetrain.GoForward(SLOW_DRIVE_RATE);
////            TMRArd_StopTimer(LINE_TRACKING_TIMER); 
//            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
//        } else if ((lineSensors.IsFrontOverLine() || lineSensors.IsCenterOverLine()) && serverBeacon.IsFacingBeacon()) {
//            drivetrain.GoForward(DRIVE_RATE);
//            currState = FOLLOWING_LINE;
//            //TMRArd_InitTimer(LINE_TRACKING_TIMER, LINE_TRACKING_TIME);
//        } else if (!lineSensors.IsCenterOverLine()){
//            SwitchSpin(true);
//            currState = SEEKING_LINE;
//        } else if (!serverBeacon.IsFacingBeacon()) {
//            currState = SEEKING_SERVER;
//        }
//        break;
//        
//    case FOLLOWING_LINE:
//        if (bumpers.IsFrontPressed() && serverBeacon.IsFacingBeacon())
//        {
//            currState = ALIGNING_WITH_WALL;
//            turntable.TurnCW(200);
//            drivetrain.GoForward(SLOW_DRIVE_RATE);
////            TMRArd_StopTimer(LINE_TRACKING_TIMER); 
//            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
//        } else if (!serverBeacon.IsFacingBeacon())
//        {
//            SwitchSpin(true);
//            currState = SEEKING_SERVER;
//        } else if(!lineSensors.IsCenterOverLine()) {
//          SwitchSpin(true);
//          currState = SEEKING_LINE;
//        } else if(!lineSensors.IsFrontOverLine() && lineSensors.IsCenterOverLine()) {
//            currState = SEEKING_LINE;
//        } 
//        break;

    case BRAKING:
        if((millis() - time < DYNAMIC_BRAKE_TIME) || TMRArd_IsTimerExpired(BRAKING_TIMER)) {
            TMRArd_ClearTimerExpired(BRAKING_TIMER);
            currState = returnToState;
            drivetrain.Stop();
            ReturnToState(returnToState);
        }
        break;

    case BACKING_UP:
        if (TMRArd_IsTimerExpired(TRAVELLING_TIMER))
        {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = returnToState;
            ReturnToState(returnToState);
        }
        break;
        //    case ALIGNING_WITH_EXCHANGE:
        //        if (exchangeBeacon.IsFacingBeacon())
        //        {
        //           drivetrain.SpinLeft(170);
        //           currState = ALIGNING_WITH_SERVER;
        //        }
        //        break;

    case GOING_TO_SERVER_WALL:
        if(bumpers.IsFrontPressed()) {
          drivetrain.GoForward(DRIVE_RATE);
          currState = ALIGNING_WITH_SERVER_WALL;
          TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
          
        }
        break;
    case ALIGNING_WITH_SERVER_WALL:
        if (TMRArd_IsTimerExpired(ALIGNING_TIMER))
        {   
            TMRArd_ClearTimerExpired(ALIGNING_TIMER);
            currState = BUTTON_PRESSING;
            TMRArd_InitTimer(BUTTON_PRESS_TIMER, BUTTON_PRESS_INTERVAL);
            drivetrain.Stop();
        }
        break;

    case BUTTON_PRESSING:
        static int numPresses = 0;
        if (numPresses == 10)
        {
            presser.Rest();
            numPresses = 0;
            
            currState = BACKING_UP;
            returnToState = SEEKING_EXCHANGE;
            TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME/2);
            drivetrain.GoBackward(DRIVE_RATE);
        }
        else
        {
            PressButton(&numPresses);
        }
        break;

    case SEEKING_EXCHANGE:
        if (exchangeBeacon.IsFacingBeacon())
        {
            currState = BRAKING;
            returnToState = TRAVELLING_TO_EXCHANGE;
            TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL);
            Brake(SPIN);
        }
        break;
        
    case TRAVELLING_TO_EXCHANGE:
        if (bumpers.IsBackPressed() || TMRArd_IsTimerExpired(TRAVELLING_TIMER))
        {
            drivetrain.GoBackward(DRIVE_RATE);
            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
            currState = ALIGNING_WITH_EXCHANGE_WALL;
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
        }
        break;
        
    case ALIGNING_WITH_EXCHANGE_WALL:
        if (TMRArd_IsTimerExpired(ALIGNING_TIMER))
        {   
            currState = DISPENSING;
            drivetrain.Stop();
            turntable.TurnCW(200);
            TMRArd_InitTimer(DISPENSER_TIMER, 10 * ONE_SEC);
            TMRArd_ClearTimerExpired(ALIGNING_TIMER);
        }
        break;
        
    case DISPENSING:
        //static int numBumperPresses = 0;
        if (TMRArd_IsTimerExpired(DISPENSER_TIMER))
        {
            turntable.Stop();
            currState = WAITING_TO_END;
        }
        break;
    case WAITING_TO_END:
        // do nothing
        break;
    default: break;
    }
}

/*--------Module Functions -------------*/
void Brake(Direction dirToBrake)
{
   if(millis() - time > DYNAMIC_BRAKE_TIME) {
    switch (dirToBrake)
    {
    case FORWARD:
        drivetrain.GoForward(DRIVE_RATE);
        break;
    case BACKWARD:
        drivetrain.GoBackward(DRIVE_RATE);
        break;
    case SPIN_LEFT:
        drivetrain.SpinLeft(SPIN_RATE);
        break;
    case SPIN_RIGHT:
        drivetrain.SpinRight(SPIN_RATE);
        break;
    case SPIN:
        if (switchSpinDir == SPIN_LEFT) {
            drivetrain.SpinLeft(SPIN_RATE);
        } else {
            drivetrain.SpinRight(SPIN_RATE);
        }
    default: break;
    }
    TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL);
  }
}

void ReturnToState(State returnToState)
{
    switch (returnToState)
    {
    case TRAVELLING_TO_SERVER:
        drivetrain.GoForward(DRIVE_RATE);
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME);
        break;
    case SEEKING_SERVER:
        SwitchSpin(true);
        break;
    case TRACKING_LINE:
        SwitchSpin(true);
        TMRArd_InitTimer(LINE_TRACKING_TIMER, LINE_TRACKING_TIME);
        break;
        
    case SEEKING_LINE:
        SwitchSpin(true);
        break;
    case ALIGNING_WITH_SERVER:
        SwitchSpin(true);
        break;
    case GOING_TO_SERVER_WALL:
        drivetrain.GoForward(DRIVE_RATE);
        break;
    case SEEKING_EXCHANGE:
        SwitchSpin(true);
        time = millis();
        break;
    case TRAVELLING_TO_EXCHANGE:   
        drivetrain.GoBackward(DRIVE_RATE);
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME*2);        break;
    default: break;
    }
}

void PressButton(int *numPresses)
{
    if (TMRArd_IsTimerExpired(BUTTON_PRESS_TIMER))
    {
        if (presser.GetPosition() == PRESS)
            presser.Withdraw();
        else
        {
            presser.Press();
            (*numPresses)++;
        }

        TMRArd_InitTimer(BUTTON_PRESS_TIMER, BUTTON_PRESS_INTERVAL);
    }
}

void TrackLine()
{

}

// pass in true if you want to switch directions
void SwitchSpin(boolean switchDir) {
    if (switchSpinDir == SPIN_LEFT)
    {
        drivetrain.SpinLeft(SPIN_RATE);
        if(switchDir) switchSpinDir = SPIN_RIGHT;
    }
    else
    {
        drivetrain.SpinRight(SPIN_RATE);
        if(switchDir) switchSpinDir = SPIN_LEFT;
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
