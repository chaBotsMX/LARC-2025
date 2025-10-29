#include"DriverController.h"

// Definición del constructor por defecto (evita el error de linkeo "undefined reference")
DriverController::DriverController() {
    // Inicialización por defecto: no se necesita acción explícita
}

void DriverController::setup(const pair pins[]) {
    for (int i = 0; i < MOTOR_COUNT; i++) {
        motors[i].setup(pins[i].first, pins[i].second);
    }
}
void DriverController::drive(int speed, int direction) {
    switch (direction) {
        case DIRECTION_FORWARD:
            forward(speed);
            break;
        case DIRECTION_BACKWARD:
            backward(speed);
            break;
        case DIRECTION_LEFT:
            left(speed);
            break;
        case DIRECTION_RIGHT:
            right(speed);
            break;
        case DIRECTION_FORWARD_RIGHT:
            forwardRight(speed);
            break;
        case DIRECTION_FORWARD_LEFT:
            forwardLeft(speed);
            break;
        case DIRECTION_BACKWARD_RIGHT:
            backwardRight(speed);
            break;
        case DIRECTION_BACKWARD_LEFT:
            backwardLeft(speed);
            break;
        default:
            stop();
            break;
    }
}

void DriverController::stop() {
    for (int i = 0; i < MOTOR_COUNT; i++) {
        motors[i].stop();
    }
}

void DriverController::forward(int speed) {
    motors[0].backward(speed);
    motors[1].forward(speed);
    motors[2].backward(speed);
    motors[3].forward(speed);
}

void DriverController::backward(int speed) {
    motors[0].backward(speed);
    motors[1].forward(speed);
    motors[2].backward(speed);
    motors[3].forward(speed);
}

void DriverController::left(int speed) {
    motors[0].backward(speed);
    motors[1].forward(speed);
    motors[2].backward(speed);
    motors[3].forward(speed);
}
void DriverController::right(int speed) {
    motors[0].forward(speed);
    motors[1].backward(speed);
    motors[2].forward(speed);
    motors[3].backward(speed);
}

void DriverController::forwardRight(int speed) {
    motors[0].stop();
    motors[1].forward(speed);
    motors[2].stop();
    motors[3].forward(speed);
}

void DriverController::forwardLeft(int speed) {
    motors[0].forward(speed);
    motors[1].stop();
    motors[2].forward(speed);
    motors[3].stop();
}
void DriverController::backwardRight(int speed) {
    motors[0].stop();
    motors[1].backward(speed);
    motors[2].stop();
    motors[3].backward(speed);
}

void DriverController::backwardLeft(int speed) {
    motors[0].backward(speed);
    motors[1].stop();
    motors[2].backward(speed);
    motors[3].stop();
}