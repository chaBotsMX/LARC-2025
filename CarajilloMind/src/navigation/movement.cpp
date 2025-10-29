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


void Movement::moveForwardTimed(int speed, unsigned long duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds() || sensors.detectObstacleAhead()) {
            emergencyStop();
            return;
        }
        motors.moveForward(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveBackwardTimed(int speed, unsigned long duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        motors.moveBackward(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveLeftTimed(int speed, unsigned long duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        motors.moveLeft(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveRightTimed(int speed, unsigned long duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        motors.moveRight(speed);
        delay(10);
    }
    motors.stop();
}

void Movement::moveForwardUntilLine(int speed) {
    while (true) {
        sensors.updateAll();
        if (sensors.isAtFrontLine()) {
            motors.stop();
            return;
        }
        if (sensors.detectObstacleAhead()) {
            emergencyStop();
            return;
        }
        motors.moveForward(speed);
        delay(10);
    }
}

void Movement::moveBackwardUntilLine(int speed) {
    while (true) {
        sensors.updateAll();
        if (sensors.isAtRearLine()) {
            motors.stop();
            return;
        }
        motors.moveBackward(speed);
        delay(10);
    }
}

void Movement::moveLeftUntilLine(int speed) {
    while (true) {
        sensors.updateAll();
        if (sensors.isAtLeftLine()) {
            motors.stop();
            return;
        }
        motors.moveLeft(speed);
        delay(10);
    }
}

void Movement::moveRightUntilLine(int speed) {
    while (true) {
        sensors.updateAll();
        if (sensors.isAtRightLine()) {
            motors.stop();
            return;
        }
        motors.moveRight(speed);
        delay(10);
    }
}

// Funciones originales con corrección simple
void Movement::moveForwardStraight(int speed, unsigned long duration) {
    updateTargetHeading();
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds() || sensors.detectObstacleAhead()) {
            emergencyStop();
            return;
        }
        
        int correction = calculateHeadingCorrection();
        motors.moveDirection(0, speed, correction);
        delay(10);
    }
    motors.stop();
}

void Movement::moveLeftStraight(int speed, unsigned long duration) {
    updateTargetHeading();
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        
        int correction = calculateHeadingCorrection();
        motors.moveDirection(-speed, 0, correction);
        delay(10);
    }
    motors.stop();
}

void Movement::moveRightStraight(int speed, unsigned long duration) {
    updateTargetHeading();
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        
        int correction = calculateHeadingCorrection();
        motors.moveDirection(speed, 0, correction);
        delay(10);
    }
    motors.stop();
}

// NUEVAS FUNCIONES CON PID
void Movement::moveForwardStraightPID(int speed, unsigned long duration) {
    updateTargetHeading();
    headingPID.setSetpoint(0); // Error = 0 cuando estamos en el rumbo correcto
    headingPID.reset();
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds() || sensors.detectObstacleAhead()) {
            emergencyStop();
            return;
        }
        
        float correction = calculateHeadingCorrectionPID();
        motors.moveDirection(0, speed, (int)correction);
        delay(10);
    }
    motors.stop();
}

void Movement::moveLeftStraightPID(int speed, unsigned long duration) {
    updateTargetHeading();
    headingPID.setSetpoint(0);
    headingPID.reset();
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        
        float correction = calculateHeadingCorrectionPID();
        motors.moveDirection(-speed, 0, (int)correction);
        delay(10);
    }
    motors.stop();
}

void Movement::moveRightStraightPID(int speed, unsigned long duration) {
    updateTargetHeading();
    headingPID.setSetpoint(0);
    headingPID.reset();
    
    unsigned long startTime = millis();
    
    while (millis() - startTime < duration) {
        sensors.updateAll();
        if (sensors.isOutOfBounds()) {
            emergencyStop();
            return;
        }
        
        float correction = calculateHeadingCorrectionPID();
        motors.moveDirection(speed, 0, (int)correction);
        delay(10);
    }
    motors.stop();
}

void Movement::emergencyStop() {
    motors.stop();
    Serial.println("EMERGENCIA: Robot detenido");
}