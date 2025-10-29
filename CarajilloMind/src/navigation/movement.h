#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "../hardware/motors.h"
#include "../hardware/sensors.h"
#include "../utils/pid.h"  // NUEVO
#include "../config.h"

class Movement {
public:
    Movement() : targetHeading(0), headingPID(1.0, 0.0, 0.0, -100, 100) {}  // Inicializar PID con valores por defecto
    void init();
    
    // Movimientos con corrección de rumbo
    void moveForwardStraight(int speed);
    void moveBackwardStraight(int speed);
    void moveLeftStraight(int speed);
    void moveRightStraight(int speed);

    // Movimiento siguiendo la línea
    void moveForwardWithLine(int speed);
    void moveBackwardWithLine(int speed);
    void moveLeftWithLine(int speed);
    void moveRightWithLine(int speed);

    void moveForwardUntilBackLine(int speed);
    void moveBackwardUntilFrontLine(int speed);
    void moveRightUntilRightLineFollowingLine(int speed);
    void moveForwardUntilFrontLineFollowingLine(int speed);
    void moveLeftUntilLeftLineFollowingLine(int speed);
    void moveBackwardUntilBackLineFollowingLine(int speed);

    void moveRightUntilRightLine(int speed);
    void moveLeftUntilLeftLine(int speed);
    
    // Movimientos con PID (más suave)
    void moveForwardStraightPID(int speed);
    void moveBackwardStraightPID(int speed);
    void moveLeftStraightPID(int speed);
    void moveRightStraightPID(int speed);

    // Rodear alberca
    void moveLeftUntilObstacleClear(int speed);
    void moveRightUntilObstacleClear(int speed);
    void moveForwardUntilObstacleClear(int speed);
    void moveBackwardUntilObstacleClear(int speed);

    // Moverte al siguiente grano de café
    // siguiendo la línea
    void moveToNextBean(int speed, int level=1);

    //Moverte a las cajas de depósito
    void moveToDepositBox(int speed, int boxNumber);

    // Parada
    void stop();
    
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