#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
#include <Arduino.h>
class MotorController {
public:
    MotorController();
    void setup(int pinA, int pinB); 
    void forward(int speed);
    void backward(int speed);
    void stop();
private:
    int pinA;
    int pinB;
};
#endif // MOTOR_CONTROLLER_H