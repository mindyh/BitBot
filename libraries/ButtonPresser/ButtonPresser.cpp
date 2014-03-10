#include "Arduino.h"
#include "ButtonPresser.h"

ButtonPresser::ButtonPresser(int pin, Servo myservo)
{
    _pin = pin;
    _myservo = myservo;
    Rest();
}

void ButtonPresser::Press() {
    _myservo.write(120);
    position = PRESS;
} 

void ButtonPresser::Withdraw() {
    _myservo.write(130);
    position = WITHDRAW;
}

void ButtonPresser::Rest() {
    _myservo.write(150);
    position = REST;
}

PresserPosition ButtonPresser::GetPosition() {
    return position;
}