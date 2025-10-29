#include "MotorController.h"

MotorController::MotorController() {}

void MotorController::setup(int pinA, int pinB) {
    this->pinA = pinA;
    this->pinB = pinB;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
}

void MotorController::forward(int speed) {
    analogWrite(pinA, speed);
    digitalWrite(pinB, HIGH);   
}

void MotorController::backward(int speed) {
    analogWrite(pinA, speed);
    digitalWrite(pinB, LOW);   
}

void MotorController::stop() {
    analogWrite(pinA, 0);
    digitalWrite(pinB, LOW);   
}
    