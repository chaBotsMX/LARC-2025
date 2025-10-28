#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../hardware/motors.h"
#include "../hardware/sensors.h"
#include "../config.h"

class Movement {
public:
    void init();
    
    // Movimientos básicos con tiempo
    void moveForwardTimed(int speed, unsigned long duration);
    void moveBackwardTimed(int speed, unsigned long duration);
    void moveLeftTimed(int speed, unsigned long duration);
    void moveRightTimed(int speed, unsigned long duration);
    
    // Movimientos hasta detectar línea
    void moveForwardUntilLine(int speed);
    void moveBackwardUntilLine(int speed);
    void moveLeftUntilLine(int speed);
    void moveRightUntilLine(int speed);
    
    // Movimientos con corrección de rumbo
    void moveForwardStraight(int speed, unsigned long duration);
    void moveLeftStraight(int speed, unsigned long duration);
    void moveRightStraight(int speed, unsigned long duration);
    
    // Parada de emergencia
    void emergencyStop();
    
private:
    float targetHeading;
    void updateTargetHeading();
    int calculateHeadingCorrection();
};

extern Movement movement;

#endif