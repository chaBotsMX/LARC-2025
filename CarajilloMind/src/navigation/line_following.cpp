#include "line_following.h"

LineFollowing lineFollowing;

void LineFollowing::init() {
    // Inicializar PID con ganancias por defecto
    // Kp, Ki, Kd, OutputMin, OutputMax
    linePID = PID(2.0, 0.1, 0.5, -100, 100);
}

void LineFollowing::setPIDGains(float kp, float ki, float kd) {
    linePID.setGains(kp, ki, kd);
    Serial.print("PID configurado: Kp=");
    Serial.print(kp);
    Serial.print(", Ki=");
    Serial.print(ki);
    Serial.print(", Kd=");
    Serial.println(kd);
}

int LineFollowing::calculateLineError(Direction dir) {
    LineSensorData line = sensors.getLineSensors();
    int error = 0;
    
    switch (dir) {
        case FORWARD:
            if (line.frontLeft && !line.frontRight) error = -1;
            else if (!line.frontLeft && line.frontRight) error = 1;
            else if (!line.frontLeft && !line.frontRight) error = 0;
            break;
            
        case BACKWARD:
            if (line.rearLeft && !line.rearRight) error = -1;
            else if (!line.rearLeft && line.rearRight) error = 1;
            else if (!line.rearLeft && !line.rearRight) error = 0;
            break;
            
        case LEFT:
            if (line.frontLeft && !line.rearLeft) error = -1;
            else if (!line.frontLeft && line.rearLeft) error = 1;
            else if (!line.frontLeft && !line.rearLeft) error = 0;
            break;
            
        case RIGHT:
            if (line.frontRight && !line.rearRight) error = -1;
            else if (!line.frontRight && line.rearRight) error = 1;
            else if (!line.frontRight && !line.rearRight) error = 0;
            break;
    }
    
    return error;
}
