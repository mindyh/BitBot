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
    
    TMRArd_InitTimer(ALIGNING_TIMER, ONE_SEC);
}

void loop()
{
    //static State currState = WAITING_TO_START;
    static State currState = ALIGNING_WITH_EXCHANGE_WALL;
    
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
            Transition(SEEKING_SERVER);
        }
        break;
    case SEEKING_SERVER:
        if (serverBeacon.IsFacingBeacon())
        {
            presser.Rest();

            //if(millis() - time > DYNAMIC_BRAKE_TIME) {
                Brake(SPIN);
                returnToState = TRAVELLING_TO_SERVER;
                currState = BRAKING;
//            } else {
//                currState = TRAVELLING_TO_SERVER;
//                Transition(TRAVELLING_TO_SERVER);
//            }
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
            if(millis() - time > DYNAMIC_BRAKE_TIME) {
                currState = BRAKING;
                returnToState = ALIGNING_WITH_SERVER;
                Brake(BACKWARD);
            } else {
                currState = ALIGNING_WITH_SERVER;
                Transition(ALIGNING_WITH_SERVER);
            }
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
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = SEEKING_SERVER;
            Transition(SEEKING_SERVER);
        }
        break;
        
    case ALIGNING_WITH_SERVER: 
        if(serverBeacon.IsFacingBeacon()) {
            if(millis() - time > DYNAMIC_BRAKE_TIME) {
                Brake(SPIN);
                currState = BRAKING;
                returnToState = GOING_TO_SERVER_WALL;
            } else {
                currState = GOING_TO_SERVER_WALL;
                Transition(GOING_TO_SERVER_WALL);
            }
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
        if(TMRArd_IsTimerExpired(BRAKING_TIMER)) {
            TMRArd_ClearTimerExpired(BRAKING_TIMER);
            currState = returnToState;
            drivetrain.Stop();
            Transition(returnToState);
        }
        break;

    case BACKING_UP:
        if (TMRArd_IsTimerExpired(TRAVELLING_TIMER))
        {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = returnToState;
            Transition(returnToState);
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
        if (numPresses == 20)
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
            if(millis() - time > DYNAMIC_BRAKE_TIME) { // if it's spinning already
                currState = BRAKING;
                returnToState = TRAVELLING_TO_EXCHANGE;
                TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL);
                Brake(SPIN);
            } else { // if it started facing the server
                currState = TRAVELLING_TO_EXCHANGE;
                Transition(TRAVELLING_TO_EXCHANGE);
            }
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
            turntable.TurnCW(TURNTABLE_RATE);
            TMRArd_InitTimer(DISPENSER_TIMER, 5 * ONE_SEC);
            drivetrain.Stop();
            TMRArd_ClearTimerExpired(ALIGNING_TIMER);
        }
        break;
        
    case DISPENSING:
        static int numTurns = 0;
        if (TMRArd_IsTimerExpired(DISPENSER_TIMER))
        {
            if(numTurns == 1) {
              numTurns = 0;
              turntable.Stop();
              currState = WAITING_TO_END;
              TMRArd_ClearTimerExpired(DISPENSER_TIMER);
            } else {
//               if(turntable.GetLastDir() == CCW) {
//                 turntable.TurnCCW(TURNTABLE_RATE);
//                 presser.Press();
//               }
//               else {
//                 presser.Rest();
//                 turntable.TurnCW(TURNTABLE_RATE);
//               }
                presser.Rest();
                turntable.TurnCCW(TURNTABLE_RATE);
                TMRArd_InitTimer(DISPENSER_TIMER, 5 * ONE_SEC);
                numTurns++;
            }
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
        break;
    default: break;
    }
    TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL);
}

void Transition(State state)
{
    switch (state)
    {
    case TRAVELLING_TO_SERVER:
        drivetrain.GoForward(DRIVE_RATE);
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME);
        time = millis();
        break;
    case SEEKING_SERVER:
        SwitchSpin(true);
        time = millis();
        break;
    // case TRACKING_LINE:
    //     SwitchSpin(true);
    //     TMRArd_InitTimer(LINE_TRACKING_TIMER, LINE_TRACKING_TIME);
    //     break;
    // case SEEKING_LINE:
    //     SwitchSpin(true);
    //     time = millis();
    //     break;
    case ALIGNING_WITH_SERVER:
        SwitchSpin(true);
        time = millis();
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
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME*2);        
        break;
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
