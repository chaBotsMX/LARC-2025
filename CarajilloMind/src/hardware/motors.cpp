#include "motors.h"

Motors motors;

void Motors::init() {
    // Configurar pines de motores como salida
    pinMode(MOTOR_FL_IN1, OUTPUT);
    pinMode(MOTOR_FL_IN2, OUTPUT);
    pinMode(MOTOR_FL_PWM, OUTPUT);
    
    pinMode(MOTOR_FR_IN1, OUTPUT);
    pinMode(MOTOR_FR_IN2, OUTPUT);
    pinMode(MOTOR_FR_PWM, OUTPUT);
    
    pinMode(MOTOR_RL_IN1, OUTPUT);
    pinMode(MOTOR_RL_IN2, OUTPUT);
    pinMode(MOTOR_RL_PWM, OUTPUT);
    
    pinMode(MOTOR_RR_IN1, OUTPUT);
    pinMode(MOTOR_RR_IN2, OUTPUT);
    pinMode(MOTOR_RR_PWM, OUTPUT);
    
    stop();
}

void Motors::setMotor(int in1, int in2, int pwmPin, int speed) {
    speed = constrainSpeed(speed);
    
    if (speed > 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(pwmPin, speed);
    } else if (speed < 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        analogWrite(pwmPin, abs(speed));
    } else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        analogWrite(pwmPin, 0);
    }
}

int Motors::constrainSpeed(int speed) {
    return constrain(speed, -255, 255);
}

void Motors::moveForward(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, speed);
}

void Motors::moveBackward(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, -speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, -speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, -speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, -speed);
}

void Motors::moveLeft(int speed) {
    // Configuración para ruedas omnidireccionales
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, -speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, -speed);
}

void Motors::moveRight(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, -speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, -speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, speed);
}

void Motors::rotateClockwise(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, -speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, -speed);
}

void Motors::rotateCounterClockwise(int speed) {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, -speed);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, speed);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, -speed);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, speed);
}

void Motors::stop() {
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, 0);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, 0);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, 0);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, 0);
}

void Motors::moveDirection(int x, int y, int rotation) {
    // Cinemática omnidireccional
    // x: movimiento lateral (-255 a 255)
    // y: movimiento adelante/atrás (-255 a 255)
    // rotation: rotación (-255 a 255)
    
    int fl = y - x - rotation;
    int fr = y + x + rotation;
    int rl = y + x - rotation;
    int rr = y - x + rotation;
    
    setMotor(MOTOR_FL_IN1, MOTOR_FL_IN2, MOTOR_FL_PWM, fl);
    setMotor(MOTOR_FR_IN1, MOTOR_FR_IN2, MOTOR_FR_PWM, fr);
    setMotor(MOTOR_RL_IN1, MOTOR_RL_IN2, MOTOR_RL_PWM, rl);
    setMotor(MOTOR_RR_IN1, MOTOR_RR_IN2, MOTOR_RR_PWM, rr);
}