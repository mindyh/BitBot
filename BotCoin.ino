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
Beacon sideServerBeacon(SIDE_SERVER_PIN, BEACON_THRESHOLD_VAL);
Beacon sideExchangeBeacon(SIDE_EXCHANGE_PIN, BEACON_THRESHOLD_VAL/2);
LineSensors lineSensors(LIGHT_PIN_CENTER, LIGHT_PIN_FRONT, LIGHT_THRESHOLD_VAL);
Drivetrain drivetrain(MOTOR_1_DIR_PIN, MOTOR_2_DIR_PIN, MOTOR_1_ENABLE_PIN, MOTOR_2_ENABLE_PIN);
Turntable turntable(TURNTABLE_ENABLE_PIN, TURNTABLE_DIR_PIN);
ButtonPresser presser(BUTTON_PRESSER_PIN, myservo);

Direction switchSpinDir = SPIN_LEFT;
static unsigned long time = 0;
static boolean isSeekingLine = false;
static SideOfServer sideOfServer;
static TargetExchange targetExchange = FIVE;
    
/*---- Main Program ---*/
void setup()
{
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    // init variables
    pinMode(HEARTBEAT_LED, OUTPUT);
    myservo.attach(BUTTON_PRESSER_PIN);
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
    static State returnToState;
    static boolean isFirstSeek = true;

    // Always do
    if (TMRArd_IsTimerExpired(HEARTBEAT_TIMER))
    {
        TMRArd_InitTimer(HEARTBEAT_TIMER, ONE_SEC);
        digitalWrite(HEARTBEAT_LED, !digitalRead(HEARTBEAT_LED));
    }

    exchangeBeacon.Update();
    serverBeacon.Update();

    if (TMRArd_IsTimerExpired(ENDGAME_TIMER))
    {
        currState = WAITING_TO_END;
        turntable.Stop();
        drivetrain.Stop();
        presser.Rest();
    }

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
        // kick to start
        if (bumpers.IsFrontPressed())
        {
            currState = SEEKING_SERVER;
            Transition(SEEKING_SERVER);
        }
        break;
    case SEEKING_SERVER:
        // found line, align with beacon
        if (lineSensors.IsCenterOverLine())
        {
            TMRArd_StopTimer(SEEKING_TIMER);
            TMRArd_ClearTimerExpired(SEEKING_TIMER);

            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)
            {
                currState = BRAKING;
                returnToState = ALIGNING_WITH_SERVER;
                Brake(SPIN, deltaT);
            }
            else
            {
                currState = ALIGNING_WITH_SERVER;
                Transition(ALIGNING_WITH_SERVER);
            }
        }
        // found the beacon, now spin left or right
        else if (serverBeacon.IsFacingBeacon()) 
        {
            // clear timer
            TMRArd_StopTimer(SEEKING_TIMER);
            TMRArd_ClearTimerExpired(SEEKING_TIMER);
            presser.Rest();

            // check what side of field you're on 
            if(sideExchangeBeacon.IsFacingBeacon()) 
                sideOfServer = RIGHT;
            else
                sideOfServer = LEFT;

            // brake if necessary
            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)
            {
                currState = BRAKING;
                returnToState = (sideOfServer == LEFT) ? SEEKING_LEFT : SEEKING_RIGHT;
                Brake(SPIN, deltaT);
            }
            else
            {
                currState = (sideOfServer == LEFT) ? SEEKING_LEFT : SEEKING_RIGHT;
                Transition(currState);
            }
        } 
        // stuck in dead zone w/ no beacon signal. 
        // Go towards exchange (guaranteed to be found)
        else if (TMRArd_IsTimerExpired(SEEKING_TIMER))
        {
            TMRArd_ClearTimerExpired(SEEKING_TIMER);

            currState = SEEKING_EXCHANGE_2;
            Transition(SEEKING_EXCHANGE_2);
        }
        break;

    case SEEKING_RIGHT:
        if(TMRArd_IsTimerExpired(SEEKING_TIMER)) {
            TMRArd_ClearTimerExpired(SEEKING_TIMER);
            currState = TRAVELLING_TO_SERVER;
            Transition(TRAVELLING_TO_SERVER);
            presser.Rest();
        }
        break;

    case SEEKING_LEFT:
        if(TMRArd_IsTimerExpired(SEEKING_TIMER)) {
            TMRArd_ClearTimerExpired(SEEKING_TIMER);
            currState = TRAVELLING_TO_SERVER;
            Transition(TRAVELLING_TO_SERVER);
            presser.Rest();
        }
        break;

    case SEEKING_EXCHANGE_2:
        // found exchange, go towards it and resume searching for server
        if(exchangeBeacon.IsFacingBeacon()) 
        {
            TMRArd_StopTimer(SEEKING_TIMER);
            TMRArd_ClearTimerExpired(SEEKING_TIMER);

            currState = BACKING_UP;
            returnToState = SEEKING_SERVER;
            Backup(1.5, BACKWARD);
        }
        // did not find (should never happen), randomly 
        // drive somewhere and try again
        else if (TMRArd_IsTimerExpired(SEEKING_TIMER)) {
            TMRArd_ClearTimerExpired(SEEKING_TIMER);

            currState = TRAVELLING_TO_SERVER;
            Transition(TRAVELLING_TO_SERVER);
        }
        break;

    case TRAVELLING_TO_SERVER:
        // Found line, align with server
        if (lineSensors.IsCenterOverLine())
        {
            TMRArd_StopTimer(TRAVELLING_TIMER);
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            
            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)
            {
                currState = BRAKING;
                returnToState = ALIGNING_WITH_SERVER;
                Brake(BACKWARD, deltaT);
            }
            else
            {
                currState = ALIGNING_WITH_SERVER;
                Transition(ALIGNING_WITH_SERVER);
            }
        }
        // hit a wall, back up and try again
        else if (bumpers.IsFrontPressed())
        {
            TMRArd_StopTimer(TRAVELLING_TIMER);
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            
            currState = BACKING_UP;
            returnToState = SEEKING_SERVER;
            Backup(1, BACKWARD);
        }
        // // the side beacon sees
        // else if(sideServerBeacon.IsFacingBeacon()) {

        // }
        // went a long time without seeing anything. Try again.
        else if (TMRArd_IsTimerExpired(TRAVELLING_TIMER) && !serverBeacon.IsFacingBeacon())
        {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);
            currState = SEEKING_SERVER;
            Transition(SEEKING_SERVER);
            //sideOfServer = (sideOfServer == LEFT) ? RIGHT : LEFT;
        }
        break;

    case ALIGNING_WITH_SERVER:
        // found beacon, go to it
        if (serverBeacon.IsFacingBeacon())
        {
            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)
            {
                currState = BRAKING;
                returnToState = GOING_TO_SERVER_WALL;
                if(sideOfServer == LEFT) {
                    Brake(SPIN_LEFT, deltaT);
                } else {
                    Brake(SPIN_RIGHT, deltaT);
                }
            }
            else
            {
                currState = GOING_TO_SERVER_WALL;
                Transition(GOING_TO_SERVER_WALL);
            }
        }
        // hit a wall, assume trouble rotating.
        // Back up and try again
        else if (bumpers.IsFrontPressed() || bumpers.IsBackPressed())
        {
            currState = BACKING_UP;
            returnToState = TRAVELLING_TO_SERVER;
            Backup(1.5, BACKWARD);
        }
        break;

    case GOING_TO_SERVER_WALL:
        // at the wall, align with it now
        if (bumpers.IsFrontPressed())
        {
            drivetrain.GoForward(DRIVE_RATE);
            currState = ALIGNING_WITH_SERVER_WALL;
            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
        }
        // went off track, try to find it again
        else if (!serverBeacon.IsFacingBeacon())
        {
            currState = ALIGNING_WITH_SERVER;
            Transition(ALIGNING_WITH_SERVER);
        }
        break;

    case ALIGNING_WITH_SERVER_WALL:
        // assume you're perfectly positioned and mine coins.
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
        // mined 5 coins. Go find an exchange.
        if (numPresses == 20)
        {
            presser.Rest();
            numPresses = 0;

            currState = BACKING_UP;
            returnToState = SEEKING_EXCHANGE;
            Backup(2, BACKWARD);
        }
        else
        {
            PressButton(&numPresses);
        }
        break;

    case SEEKING_EXCHANGE:
        // found exchange, go towards it
        if (exchangeBeacon.IsFacingBeacon())
        {
            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)  // if it's spinning already
            {
                currState = BRAKING;
                returnToState = TRAVELLING_TO_EXCHANGE;
                Brake(SPIN, deltaT);
            }
            else // if it started facing the server
            {
                currState = TRAVELLING_TO_EXCHANGE;
                Transition(TRAVELLING_TO_EXCHANGE);
            }
            TMRArd_StopTimer(SEEKING_TIME);
            TMRArd_ClearTimerExpired(SEEKING_TIMER);
        }
        // cannot find any exchanges. Align with the server
        // and try agin
        else if (TMRArd_IsTimerExpired(SEEKING_TIMER))
        {
            TMRArd_ClearTimerExpired(SEEKING_TIMER);
            currState = SEEKING_SERVER_2;
            Transition(SEEKING_SERVER_2);
        }
        break;

//    case ALIGNING_SIDE_WITH_SERVER:
//        if(sideServerBeacon.IsFacingBeacon()) {
//            if (millis() - time > DYNAMIC_BRAKE_TIME)
//            {
//                currState = BRAKING;
//                returnToState = TRAVELLING_TO_EXCHANGE_2;
//                Brake(SPIN);
//            }
//            else
//            {
//                currState = TRAVELLING_TO_EXCHANGE_2;
//                Transition(TRAVELLING_TO_EXCHANGE_2);
//            }
//        }
//        break;
//
//    case TRAVELLING_TO_EXCHANGE_2:
//        currState = BRAKING;
//        returnToState = SEEKING_EXCHANGE;
//        if(targetExchange == FIVE) {
//            if(sideOfServer == LEFT) 
//                Brake(FORWARD);
//            else 
//                Brake(BACKWARD);
//        } else if (targetExchange == THREE) {
//            if(sideOfServer == LEFT) 
//                Brake(BACKWARD);
//            else 
//                Brake(FORWARD);
//        }
//        break;
//
//    case SEEKING_SERVER_2:
//        // found server. Go towards it a bit to 
//        // give yourself room to find exchange beacons.
//        if (serverBeacon.IsFacingBeacon())
//        {
//            // test to see if dynamic braking is needed
//            currState = BACKING_UP;
//            returnToState = SEEKING_EXCHANGE;
//            Backup(3, FORWARD);
//        }
//        break;

    case TRAVELLING_TO_EXCHANGE:
        // lost track of the exchange. Find another one.
        if (!exchangeBeacon.IsFacingBeacon())
        {
            TMRArd_StopTimer(TRAVELLING_TIMER);
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);

            currState = SEEKING_EXCHANGE;
            Transition(SEEKING_EXCHANGE);
        }
        // at the exchange. Align with it before dropping coins.
        else if (bumpers.IsBackPressed() || TMRArd_IsTimerExpired(TRAVELLING_TIMER))
        {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);

            drivetrain.GoBackward(DRIVE_RATE);
            TMRArd_InitTimer(ALIGNING_TIMER, ALIGNING_TIME);
            currState = ALIGNING_WITH_EXCHANGE_WALL;
        }
        break;

    case ALIGNING_WITH_EXCHANGE_WALL:
        // assume you're perfectly aligned. Start dispensing.
        if (TMRArd_IsTimerExpired(ALIGNING_TIMER))
        {
            drivetrain.Stop();
            TMRArd_ClearTimerExpired(ALIGNING_TIMER);

            currState = DISPENSING;
            turntable.TurnCW(TURNTABLE_RATE);
            TMRArd_InitTimer(DISPENSER_TIMER, 5 * ONE_SEC);
        }
        break;

    case DISPENSING:
        static int numTurns = 0;
        if (TMRArd_IsTimerExpired(DISPENSER_TIMER))
        {
            //  After you're done dispensing, back up and 
            // mine more coins
            if (numTurns == 2)
            {
                numTurns = 0;
                turntable.Stop();
                TMRArd_ClearTimerExpired(DISPENSER_TIMER);

                currState = BACKING_UP;
                returnToState = SEEKING_SERVER;
                Backup(1.5, FORWARD);
            }
            // switch direction of dispenser every so often,
            // just in case.
            else
            {
                if (turntable.GetLastDir() == CCW)
                {
                    turntable.TurnCCW(TURNTABLE_RATE);
                }
                else
                {
                    turntable.TurnCW(TURNTABLE_RATE);
                }
                TMRArd_InitTimer(DISPENSER_TIMER, 5 * ONE_SEC);
                numTurns++;
            }
        }
        break;
    case WAITING_TO_END:
        // do nothing
        break;

    // special state, returns to whatever state was 
    // set in returnToState
    case BRAKING:
        if (TMRArd_IsTimerExpired(BRAKING_TIMER))
        {
            TMRArd_ClearTimerExpired(BRAKING_TIMER);
            currState = returnToState;
            drivetrain.Stop();
            Transition(returnToState);
        }
        break;

    case BACKING_UP:
        // only stop backing up if we're looking for the line 
        // and found it
        if (isSeekingLine && lineSensors.IsCenterOverLine())
        {
            int deltaT = millis() - time;
            if (deltaT > DYNAMIC_BRAKE_TIME)
            {
                currState = BRAKING;
                returnToState = ALIGNING_WITH_SERVER;
                Brake(FORWARD, deltaT);
            }
            else
            {
                currState = ALIGNING_WITH_SERVER;
                Transition(ALIGNING_WITH_SERVER);
            }
        }
        else if (TMRArd_IsTimerExpired(TRAVELLING_TIMER))
        {
            TMRArd_ClearTimerExpired(TRAVELLING_TIMER);

            currState = returnToState;
            Transition(returnToState);
        }
        break;

    default: break;
    }
}

/*--------Module Functions -------------*/

/* ALWAYS set currState = BACKING_UP and returnToState before calling */
void Backup(float duration, Direction dir)
{
    time = millis();
    TMRArd_InitTimer(BACKING_UP_TIMER, ONE_SEC * duration);
    if (dir == BACKWARD)
        drivetrain.GoBackward(DRIVE_RATE);
    else
        drivetrain.GoForward(DRIVE_RATE);
}

/* ALWAYS set currState = BRAKING and returnToState before calling */
void Brake(Direction dirToBrake, int deltaT)
{
    if(deltaT > FAST_BRAKE_TIME)
        TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL*2);
    else 
        TMRArd_InitTimer(BRAKING_TIMER, BRAKING_INTERVAL);

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
        if (switchSpinDir == SPIN_LEFT)
        {
            drivetrain.SpinLeft(SPIN_RATE);
        }
        else
        {
            drivetrain.SpinRight(SPIN_RATE);
        }
        break;
    default: break;
    }
}

// Transitions to each state.
// Does not include Braking and Backing up.
void Transition(State state)
{
    switch (state)
    {
    case SEEKING_SERVER:
        TMRArd_InitTimer(SEEKING_TIMER, SEEKING_TIME);
        SwitchSpin();
        time = millis();
        isSeekingLine = true;
        break;

    case SEEKING_RIGHT:
        TMRArd_InitTimer(SEEKING_TIMER, ONE_SEC/3);
        drivetrain.SpinRight(SPIN_RATE);
        break;

    case SEEKING_LEFT:
        TMRArd_InitTimer(SEEKING_TIMER, ONE_SEC/3);
        drivetrain.SpinLeft(SPIN_RATE);
        break;

    case SEEKING_EXCHANGE_2:
        TMRArd_InitTimer(SEEKING_TIMER, SEEKING_TIME);
        SwitchSpin();
        time = millis();
        isSeekingLine = true;
        break;

    case TRAVELLING_TO_SERVER:
        drivetrain.GoForward(DRIVE_RATE);
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME*2);
        time = millis();
        isSeekingLine = true;
        break;

    case ALIGNING_WITH_SERVER:
        if(sideOfServer == LEFT) {
            drivetrain.SpinRight(SPIN_RATE);
        } else {
            drivetrain.SpinLeft(SPIN_RATE);
        }
        time = millis();
        isSeekingLine = false;
        break;

    case GOING_TO_SERVER_WALL:
        drivetrain.GoForward(DRIVE_RATE);
        break;

    case SEEKING_EXCHANGE:
        TMRArd_InitTimer(SEEKING_TIMER, SEEKING_TIME);
        time = millis();
        if(targetExchange == FIVE) {
            if(sideOfServer == LEFT) 
                drivetrain.SpinRight(SPIN_RATE);
            else 
                drivetrain.SpinLeft(SPIN_RATE);
        } else if (targetExchange == THREE) {
            if(sideOfServer == LEFT) 
                drivetrain.SpinLeft(SPIN_RATE);
            else 
                drivetrain.SpinRight(SPIN_RATE);
        }
        break;

    case TRAVELLING_TO_EXCHANGE:
        drivetrain.GoBackward(DRIVE_RATE);
        TMRArd_InitTimer(TRAVELLING_TIMER, TRAVELLING_TIME * 4);
        break;

    case SEEKING_SERVER_2:
        SwitchSpin();
        time = millis();
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

void SwitchSpin()
{
    if (switchSpinDir == SPIN_LEFT)
    {
        drivetrain.SpinLeft(SPIN_RATE);
        switchSpinDir = SPIN_RIGHT;
    }
    else
    {
        drivetrain.SpinRight(SPIN_RATE);
        switchSpinDir = SPIN_LEFT;
    }
}
