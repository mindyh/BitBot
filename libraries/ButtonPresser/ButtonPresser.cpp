#include "Arduino.h"
#include "ButtonPresser.h"

ButtonPresser::ButtonPresser(int pin, Servo myservo)
{
    _pin = pin;
    _myservo = myservo;
    Rest();
}

void ButtonPresser::Press() {
    _myservo.write(60);
    position = PRESS;
} 

void ButtonPresser::Withdraw() {
    _myservo.write(90);
    position = WITHDRAW;
}

void ButtonPresser::Rest() {
    _myservo.write(110);
    position = REST;
}

PresserPosition ButtonPresser::GetPosition() {
    return position;
}