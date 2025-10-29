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

// NUEVA FUNCIÓN: Error analógico más preciso usando valores crudos
int LineFollowing::calculateLineErrorAnalog(Direction dir) {
    int leftValue, rightValue;
    
    switch (dir) {
        case FORWARD:
            leftValue = analogRead(LINE_FRONT_LEFT);
            rightValue = analogRead(LINE_FRONT_RIGHT);
            break;
            
        case BACKWARD:
            leftValue = analogRead(LINE_REAR_LEFT);
            rightValue = analogRead(LINE_REAR_RIGHT);
            break;
            
        case LEFT:
            leftValue = analogRead(LINE_FRONT_LEFT);
            rightValue = analogRead(LINE_REAR_LEFT);
            break;
            
        case RIGHT:
            leftValue = analogRead(LINE_FRONT_RIGHT);
            rightValue = analogRead(LINE_REAR_RIGHT);
            break;
    }
    
    // Calcular error proporcional basado en diferencia de valores
    // Valores más bajos = más negro (más cerca de la línea)
    int error = rightValue - leftValue;
    
    // Normalizar error a rango aproximado -100 a 100
    error = constrain(error, -500, 500);
    error = map(error, -500, 500, -100, 100);
    
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

// NUEVA FUNCIÓN: Corrección con PID
void LineFollowing::correctPositionPID(Direction dir, float correction, int baseSpeed) {
    int correctionInt = (int)correction;
    
    switch (dir) {
        case FORWARD:
            motors.moveDirection(correctionInt, baseSpeed, 0);
            break;
        case BACKWARD:
            motors.moveDirection(correctionInt, -baseSpeed, 0);
            break;
        case LEFT:
            motors.moveDirection(-baseSpeed, correctionInt, 0);
            break;
        case RIGHT:
            motors.moveDirection(baseSpeed, correctionInt, 0);
            break;
    }
}

// Funciones originales (sin PID) - se mantienen para compatibilidad
void LineFollowing::followLineForward(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    while (true) {
        sensors.updateAll();
        
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
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

// NUEVAS FUNCIONES CON PID
void LineFollowing::followLineForwardPID(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    // Resetear PID al inicio
    linePID.setSetpoint(0); // Setpoint = 0 (centrado en la línea)
    linePID.reset();
    
    while (true) {
        sensors.updateAll();
        
        if (maxDuration > 0 && (millis() - startTime > maxDuration)) {
            motors.stop();
            return;
        }
        
        if (sensors.isAtFrontLine() || sensors.detectObstacleAhead()) {
            motors.stop();
            return;
        }
        
        // Obtener error analógico y calcular corrección PID
        int error = calculateLineErrorAnalog(FORWARD);
        float correction = linePID.compute(error);
        
        correctPositionPID(FORWARD, correction, speed);
        
        delay(10);
    }
}

void LineFollowing::followLineLeftPID(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    linePID.setSetpoint(0);
    linePID.reset();
    
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
        
        int error = calculateLineErrorAnalog(LEFT);
        float correction = linePID.compute(error);
        
        correctPositionPID(LEFT, correction, speed);
        
        delay(10);
    }
}

void LineFollowing::followLineRightPID(int speed, unsigned long maxDuration) {
    unsigned long startTime = millis();
    
    linePID.setSetpoint(0);
    linePID.reset();
    
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
        
        int error = calculateLineErrorAnalog(RIGHT);
        float correction = linePID.compute(error);
        
        correctPositionPID(RIGHT, correction, speed);
        
        delay(10);
    }
}

void LineFollowing::alignWithHorizontalLine() {
    sensors.updateAll();
    LineSensorData line = sensors.getLineSensors();
    
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
    return (line.frontLeft && line.frontRight && line.rearLeft && line.rearRight);
}

bool LineFollowing::isAtCorner() {
    LineSensorData line = sensors.getLineSensors();
    int count = line.frontLeft + line.frontRight + line.rearLeft + line.rearRight;
    return (count == 2 || count == 3);
}