#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../hardware/motors.h"
#include "../hardware/sensors.h"
#include "../utils/pid.h"  // NUEVO
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
    
    // Movimientos con PID (más suave)
    void moveForwardStraightPID(int speed, unsigned long duration);  // NUEVO
    void moveLeftStraightPID(int speed, unsigned long duration);     // NUEVO
    void moveRightStraightPID(int speed, unsigned long duration);    // NUEVO
    
    // Parada de emergencia
    void emergencyStop();
    
    // Configuración PID
    void setHeadingPIDGains(float kp, float ki, float kd);  // NUEVO
    
private:
    float targetHeading;
    PID headingPID;  // NUEVO
    
    void updateTargetHeading();
    int calculateHeadingCorrection();
    float calculateHeadingCorrectionPID();  // NUEVO
};

extern Movement movement;

#endif