#include "movement.h"

Movement movement;

void Movement::init() {
    // Inicializar PID de rumbo
    // Kp, Ki, Kd, OutputMin, OutputMax
    headingPID = PID(2.0, 0.05, 0.3, -80, 80);
    updateTargetHeading();
}

void Movement::setHeadingPIDGains(float kp, float ki, float kd) {
    headingPID.setGains(kp, ki, kd);
    Serial.print("Heading PID configurado: Kp=");
    Serial.print(kp);
    Serial.print(", Ki=");
    Serial.print(ki);
    Serial.print(", Kd=");
    Serial.println(kd);
}

void Movement::updateTargetHeading() {
    targetHeading = sensors.getHeading();
}

int Movement::calculateHeadingCorrection() {
    float currentHeading = sensors.getHeading();
    float error = targetHeading - currentHeading;
    
    // Normalizar error a -180 a 180
    if (error > 180) error -= 360;
    if (error < -180) error += 360;
    
    // PID simple (solo proporcional)
    int correction = (int)(error * 2.0); // Kp = 2.0
    return constrain(correction, -50, 50);
}

// NUEVA FUNCIÓN: Corrección PID completa
float Movement::calculateHeadingCorrectionPID() {
    float currentHeading = sensors.getHeading();
    float error = targetHeading - currentHeading;
    
    // Normalizar error a -180 a 180
    if (error > 180) error -= 360;
    if (error < -180) error += 360;
    
    return headingPID.compute(error);
}


void Movement::moveForwardStraight(int speed) {
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        motors.moveForward(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveBackwardStraight(int speed) {
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        motors.moveBackward(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveLeftStraight(int speed) {
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        motors.moveLeft(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveRightStraight(int speed) {
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        motors.moveRight(speed);
        delay(10);
    }
    motors.stop();
}   


/*
void Movement::moveRightStraightPID(int speed) {
    updateTargetHeading();
    headingPID.setSetpoint(0);
    headingPID.reset();
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < 6000) {
        sensors.updateAll();
        
        float correction = calculateHeadingCorrectionPID();
        motors.moveDirection(speed, 0, (int)correction);
        delay(10);
    }
    motors.stop();
}

*/

void Movement::stop() {
    motors.stop();
    Serial.println("Robot detenido");
}