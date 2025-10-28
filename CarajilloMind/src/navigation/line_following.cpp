#include "line_following.h"

LineFollowing lineFollowing;

void LineFollowing::init() {
    // Nada especial que inicializar
}

int LineFollowing::calculateLineError(Direction dir) {
    LineSensorData line = sensors.getLineSensors();
    int error = 0;
    
    switch (dir) {
        case FORWARD:
            // Error basado en sensores frontales
            if (line.frontLeft && !line.frontRight) error = -1;
            else if (!line.frontLeft && line.frontRight) error = 1;
            else if (!line.frontLeft && !line.frontRight) error = 0; // Centrado
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

void LineFollowing::correctPosition(Direction dir, int error, int baseSpeed) {
    int correction = error * 30; // Factor de corrección
    
    switch (dir) {
        case FORWARD:
            motors.moveDirection(correction, baseSpeed, 0);
            break;
        case BACKWARD:
            motors.moveDirection(correction, -baseSpeed, 0);
            break;
        case LEFT:
            motors.moveDirection(-baseSpeed, correction, 0);
            break;
        case RIGHT:
            motors.moveDirection(baseSpeed, correction, 0);
            break;
    }
}

void LineFollowing::followLineForward(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    while (true) {
        sensors.updateAll();
        
        // Verificar timeout
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
        // Verificar condiciones de parada
        if (sensors.isAtFrontLine() || sensors.detectObstacleAhead()) {
            motors.stop();
            return;
        }
        
        int error = calculateLineError(FORWARD);
        correctPosition(FORWARD, error, speed);
        
        delay(10);
    }
}

void LineFollowing::followLineBackward(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    while (true) {
        sensors.updateAll();
        
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
        if (sensors.isAtRearLine()) {
            motors.stop();
            return;
        }
        
        int error = calculateLineError(BACKWARD);
        correctPosition(BACKWARD, error, speed);
        
        delay(10);
    }
}

void LineFollowing::followLineLeft(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    while (true) {
        sensors.updateAll();
        
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
        if (sensors.isAtLeftLine()) {
            motors.stop();
            return;
        }
        
        int error = calculateLineError(LEFT);
        correctPosition(LEFT, error, speed);
        
        delay(10);
    }
}

void LineFollowing::followLineRight(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    while (true) {
        sensors.updateAll();
        
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
        if (sensors.isAtRightLine()) {
            motors.stop();
            return;
        }
        
        int error = calculateLineError(RIGHT);
        correctPosition(RIGHT, error, speed);
        
        delay(10);
    }
}

void LineFollowing::alignWithHorizontalLine() {
    // Pequeños ajustes para quedar perpendicular a línea horizontal
    sensors.updateAll();
    LineSensorData line = sensors.getLineSensors();
    
    // Si solo un sensor frontal detecta, girar hasta que ambos detecten
    while (!(line.frontLeft && line.frontRight)) {
        sensors.updateAll();
        line = sensors.getLineSensors();
        
        if (line.frontLeft && !line.frontRight) {
            motors.rotateClockwise(ALIGN_SPEED);
        } else if (!line.frontLeft && line.frontRight) {
            motors.rotateCounterClockwise(ALIGN_SPEED);
        } else {
            motors.stop();
            break;
        }
        delay(10);
    }
    motors.stop();
}

void LineFollowing::alignWithVerticalLine() {
    sensors.updateAll();
    LineSensorData line = sensors.getLineSensors();
    
    // Alinear con línea vertical (izquierda o derecha)
    while (!(line.frontLeft && line.rearLeft) && !(line.frontRight && line.rearRight)) {
        sensors.updateAll();
        line = sensors.getLineSensors();
        
        if ((line.frontLeft && !line.rearLeft) || (line.frontRight && !line.rearRight)) {
            motors.rotateClockwise(ALIGN_SPEED);
        } else if ((!line.frontLeft && line.rearLeft) || (!line.frontRight && line.rearRight)) {
            motors.rotateCounterClockwise(ALIGN_SPEED);
        } else {
            motors.stop();
            break;
        }
        delay(10);
    }
    motors.stop();
}

bool LineFollowing::isAtIntersection() {
    LineSensorData line = sensors.getLineSensors();
    // Intersección: todos los sensores detectan línea
    return (line.frontLeft && line.frontRight && line.rearLeft && line.rearRight);
}

bool LineFollowing::isAtCorner() {
    LineSensorData line = sensors.getLineSensors();
    int count = line.frontLeft + line.frontRight + line.rearLeft + line.rearRight;
    // Esquina: 2 o 3 sensores detectan línea
    return (count == 2 || count == 3);
}