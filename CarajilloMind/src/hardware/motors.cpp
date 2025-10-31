#include "motors.h"

Motors motors;

void Motors::init() {
    // Configurar pines de motores como salida
    pinMode(MOTOR_FL_IN1, OUTPUT);
    pinMode(MOTOR_FL_IN2, OUTPUT);
    
    pinMode(MOTOR_FR_IN1, OUTPUT);
    pinMode(MOTOR_FR_IN2, OUTPUT);
    
    pinMode(MOTOR_RL_IN1, OUTPUT);
    pinMode(MOTOR_RL_IN2, OUTPUT);
    
    pinMode(MOTOR_RR_IN1, OUTPUT);
    pinMode(MOTOR_RR_IN2, OUTPUT);
    
    stop();
}

void Motors::setMotor(int in1, int in2, int speed) {
    speed = constrainSpeed(speed);
    if (speed > 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
    } else if (speed < 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
    } else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
    }
}

int Motors::constrainSpeed(int speed) {
    return constrain(speed, -255, 255);
}

void Motors::moveForward(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, 0);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, 0);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, speed);
}

void Motors::moveBackward(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, -speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, 0);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, 0);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, -speed);
}

void Motors::moveLeft(int speed) {
    // Configuración para ruedas omnidireccionales
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, 0);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, 0);
}

void Motors::moveRight(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, 0);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, -speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, -speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, 0);
}

void Motors::stop() {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, 0);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, 0);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, 0);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, 0);
}