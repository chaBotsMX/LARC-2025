#include "movement.h"

Movement movement;

void Movement::init() {
    // Inicializar PID de rumbo
    // Kp, Ki, Kd, OutputMin, OutputMax
    headingPID = PID(2.0, 0.05, 0.3, -80, 80);
    updateTargetHeading();
}

/*
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
*/

void Movement::moveForwardStraight(int speed) {
    
    unsigned long startTime = 0;
    static bool running = false;

    if (!running) {
        startTime = millis();
        motors.moveForward(speed);
        running = true;
    }
    
    if (millis() - startTime >= 5000) {
        motors.stop();
        running = false;
    }
}

void Movement::moveBackwardStraight(int speed) {
    unsigned long startTime = 0;
    static bool running = false;

    if (!running) {
        startTime = millis();
        motors.moveBackward(speed);
        running = true;
    }

    if (millis() - startTime >= 5000) {
        motors.stop();
        running = false;
    }
}

void Movement::moveLeftStraight(int speed) {
    unsigned long startTime = 0;
    static bool running = false;

    if (!running) {
        startTime = millis();
        motors.moveLeft(speed);
        running = true;
    }

    if (millis() - startTime >= 5000) {
        motors.stop();
        running = false;
    }
}


void Movement::moveRightStraight(int speed) {
    unsigned long startTime = 0;
    static bool running = false;

    if (!running) {
        startTime = millis();
        motors.moveRight(speed);
        running = true;
    }
    
    if (millis() - startTime >= 5000) {
        motors.stop();
        running = false;
    }
    
}   

void Movement::moveRightUntilRightLineFollowingLine(int speed) {
    static bool running = false;
    static unsigned long searchTimer = 0;
    static enum State { MOVING, SEARCHING } state = MOVING;

    LineSensorData ls = sensors.getLineSensors();
    bool RL = ls.rearLeft;
    bool RR = ls.rearRight;

    // Condición de parada
    if (lineFollowing.reachedRightLine()) {
        motors.stop();
        running = false;
        state = MOVING;   // reset
        return;
    }

    if (!running) {
        running = true;
        motors.moveRight(speed);
        return;
    }

    switch (state) {
        // ==========================
        case MOVING:
        // ==========================
            // Caso ideal: centrado
            if (RL && RR) {
                motors.moveRight(speed);
            }

            // Línea ligeramente a la izquierda
            else if (RL && !RR) {
                motors.moveForward(speed * 0.6);
            }

            // Línea ligeramente a la derecha
            else if (!RL && RR) {
                motors.moveBackward(speed * 0.6);
            }

            // Perdió la línea
            else if (!RL && !RR) {
                state = SEARCHING;
                searchTimer = millis();
            }
            break;

        // ==========================
        case SEARCHING:
        // ==========================
            // Busca adelante primero
            motors.moveForward(speed * 0.4);

            // Si no la encuentra en 400ms, intenta atrás
            if (millis() - searchTimer > 400) {
                motors.moveBackward(speed * 0.4);
            }

            // ¿La recuperó?
            if (RL || RR) {
                state = MOVING;
            }
            break;
    }
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