#ifndef LINE_FOLLOWING_H
#define LINE_FOLLOWING_H

#include "../hardware/motors.h"
#include "../hardware/sensors.h"
#include "../config.h"

class LineFollowing {
public:
    void init();
    
    // Seguimiento de línea básico
    void followLineForward(int speed, unsigned long maxDuration = 0);
    void followLineBackward(int speed, unsigned long maxDuration = 0);
    void followLineLeft(int speed, unsigned long maxDuration = 0);
    void followLineRight(int speed, unsigned long maxDuration = 0);
    
    // Alineación con líneas
    void alignWithHorizontalLine();
    void alignWithVerticalLine();
    
    // Detección de posición
    bool isAtIntersection();
    bool isAtCorner();
    
private:
    int calculateLineError(Direction dir);
    void correctPosition(Direction dir, int error, int baseSpeed);
};

extern LineFollowing lineFollowing;

#endif