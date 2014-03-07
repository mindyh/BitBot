/* BotCoin.h
 * ------------
 * Mindy Huang (mindyh@stanford.edu)
 */

/*---------------- Defines ---------------------------*/
#ifndef BOTCOIN_H
#define BOTCOIN_H

#include "Arduino.h"

// Digital Pins
#define TURNTABLE_DIR_PIN       2
#define TURNTABLE_ENABLE_PIN    3  
#define MOTOR_2_DIR_PIN         4
#define MOTOR_2_ENABLE_PIN      5
#define MOTOR_1_ENABLE_PIN      6
#define MOTOR_1_DIR_PIN         7
#define BUTTON_PRESSER_PIN      8
#define BUMPER_BR_PIN           9
#define BUMPER_BL_PIN           10
#define BUMPER_FL_PIN           11
#define BUMPER_FR_PIN           12
#define HEARTBEAT_LED           13

// Analog Pins
#define LIGHT_PIN_FRONT         A0
#define LIGHT_PIN_CENTER        A1
#define BEACON_SERVER_PIN       A2
#define BEACON_EXCHANGE_PIN     A3

// Timers
#define HEARTBEAT_TIMER         1
#define ENDGAME_TIMER           2
#define CLEAR_BEACON_TIMER      3
#define DISPENSER_TIMER         4
#define BUTTON_PRESS_TIMER      5
#define BRAKING_TIMER           6
#define TRAVELLING_TIMER        7
#define LINE_TRACKING_TIMER     8
#define ALIGNING_TIMER          9
#define SEEKING_TIMER           10
#define BACKING_UP_TIMER        11

// Values
#define ONE_SEC               1000
#define BEACON_CLEAR_PERIOD   ONE_SEC/20
#define BUTTON_PRESS_INTERVAL ONE_SEC/4
#define BEACON_THRESHOLD_VAL  400
#define LIGHT_THRESHOLD_VAL   200 
#define BRAKING_INTERVAL      ONE_SEC/4
#define TRAVELLING_TIME       2.5*ONE_SEC 
#define SPIN_RATE             150
#define SLOW_DRIVE_RATE       150
#define DRIVE_RATE            180
#define TURNTABLE_RATE        240
#define LINE_TRACKING_TIME    2*ONE_SEC
#define ALIGNING_TIME         ONE_SEC*2
#define DYNAMIC_BRAKE_TIME    ONE_SEC/2
#define SEEKING_TIME          4*ONE_SEC

enum Direction {
    FORWARD,
    BACKWARD,
    SPIN_LEFT,
    SPIN_RIGHT,
    SPIN
};

// side of the robot
enum Side {
    FRONT,
    BACK
};

// states
enum State {
    WAITING_TO_START,
    SEEKING_SERVER,
    TRAVELLING_TO_SERVER,
    ALIGNING_WITH_EXCHANGE_WALL,
    ALIGNING_WITH_SERVER_WALL,
    GOING_TO_SERVER_WALL,
    TRACKING_LINE,
    BUTTON_PRESSING,
    SEEKING_EXCHANGE,
    TRAVELLING_TO_EXCHANGE,
    DISPENSING,
    WAITING_TO_END,
    BACKING_UP,
    BRAKING,
    SEEKING_LINE,
    ALIGNING,
    FOLLOWING_LINE,
    ALIGNING_WITH_SERVER,
    INITIAL_ALIGNING_WITH_EXCHANGE
};

enum LineTrackingState {

};

enum LineTurningDir {
    LEFT,
    RIGHT
};

enum Bumper {
    FL,
    FR,
    BL,
    BR
};
  

#endif
