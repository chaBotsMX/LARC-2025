#ifndef LINE_FOLLOWING_H
#define LINE_FOLLOWING_H

#include "../hardware/motors.h"
#include "../hardware/sensors.h"
#include "../utils/pid.h"
#include "../config.h"

class LineFollowing {
public:
    void init();

    //Detección de líneas
    bool reachedRightLine();
    bool reachedLeftLine();
    
    // Seguimiento de línea básico
    void followLineForward(int speed, unsigned long maxDuration = 0);
    void followLineBackward(int speed, unsigned long maxDuration = 0);
    void followLineLeft(int speed, unsigned long maxDuration = 0);
    void followLineRight(int speed, unsigned long maxDuration = 0);
    
    // Seguimiento con PID (más suave y preciso)
    void followLineForwardPID(int speed, unsigned long maxDuration = 0);
    void followLineLeftPID(int speed, unsigned long maxDuration = 0);
    void followLineRightPID(int speed, unsigned long maxDuration = 0);
    
    // Alineación con líneas
    void alignWithHorizontalLine();
    void alignWithVerticalLine();
    
    // Detección de posición
    bool isAtIntersection();
    bool isAtCorner();
    
    // Configuración de PID
    void setPIDGains(float kp, float ki, float kd);
    
private:
    int calculateLineError(Direction dir);
    int calculateLineErrorAnalog(Direction dir);  // NUEVO: error analógico más preciso
    void correctPosition(Direction dir, int error, int baseSpeed);
    void correctPositionPID(Direction dir, float correction, int baseSpeed);  // NUEVO
    
    // Controlador PID
    PID linePID;  // NUEVO
};

extern LineFollowing lineFollowing;

#endif