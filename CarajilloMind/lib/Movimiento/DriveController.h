#ifndef DRIVER_CONTROLLER_H
#define DRIVER_CONTROLLER_H
#include "MotorController.h"

class DriverController {
public:
    struct pair {
        int first;
                int second;
    };
    DriverController();
    const static int DIRECTION_FORWARD = 0;
    const static int DIRECTION_BACKWARD = 1;
    const static int DIRECTION_LEFT = 2;
    const static int DIRECTION_RIGHT = 3;
    const static int DIRECTION_FORWARD_RIGHT = 4;
    const static int DIRECTION_FORWARD_LEFT = 5;
    const static int DIRECTION_BACKWARD_RIGHT = 6;
    const static int DIRECTION_BACKWARD_LEFT = 7;
    void setup(const pair pins[]);
    void drive(int speed, int direction);
    void stop();
private:
    const static int MOTOR_COUNT = 4;
    MotorController motors[MOTOR_COUNT];
    void forward(int speed);
    void backward(int speed);
    void left(int speed);
    void right(int speed);
    void forwardRight(int speed);
    void forwardLeft(int speed);
    void backwardRight(int speed);
    void backwardLeft(int speed);
};
#endif // DRIVER_CONTROLLER_H