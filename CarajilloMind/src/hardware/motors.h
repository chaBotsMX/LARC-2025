#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include "../config.h"

class Motors {
public:
    void init();
    void moveForward(int speed);
    void moveBackward(int speed);
    void moveLeft(int speed);
    void moveRight(int speed);
    void stop();
    
private:
    void setMotor(int in1, int in2, int speed);
    int constrainSpeed(int speed);
};

extern Motors motors;

#endif