#ifndef OBSTACLE_AVOIDANCE_H
#define OBSTACLE_AVOIDANCE_H

#include <Arduino.h>
#include "../drivers/motors.h"
#include "../drivers/sensors.h"
#include "../navigation/movement.h"
#include "../config/constants.h"

// ============================================
// ENUMERACIONES
// ============================================

enum class ObstaclePosition {
    NONE,
    FRONT,
    FRONT_LEFT,
    FRONT_RIGHT,
    BOTH_SIDES,
    UNKNOWN
};

// ============================================
// CLASE DE EVASIÓN DE OBSTÁCULOS
// ============================================

class ObstacleAvoidance {
public:
    ObstacleAvoidance(MotorController& motors, DistanceSensors& distance_sensors, 
                      LineSensors& line_sensors, Movement& movement);
    
    // Inicialización
    void begin();
    
    // Detección
    bool checkForObstacles();
    ObstaclePosition getObstaclePosition();
    bool isPathClear();
    
    // Navegación específica para el campo
    bool detectOpenPathBetweenPools();
    bool navigateThroughPools(uint8_t speed = Speed::SLOW_SPEED);
    bool alignWithTree();
    
    // Evasión automática
    bool avoidObstacle(uint8_t speed = Speed::SLOW_SPEED);
    
    // Seguridad
    bool emergencyStop();
    
    // Estado
    uint16_t getFrontLeftDistance() const;
    uint16_t getFrontRightDistance() const;
    void printObstacleStatus();
    
private:
    MotorController& motors;
    DistanceSensors& distance_sensors;
    LineSensors& line_sensors;
    Movement& movement;
    
    ObstaclePosition last_obstacle_position;
    
    // Helpers
    ObstaclePosition analyzeDistances();
    bool waitForClearPath(unsigned long timeout_ms = 5000);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline ObstacleAvoidance::ObstacleAvoidance(MotorController& motors, 
                                             DistanceSensors& distance_sensors,
                                             LineSensors& line_sensors,
                                             Movement& movement)
    : motors(motors),
      distance_sensors(distance_sensors),
      line_sensors(line_sensors),
      movement(movement),
      last_obstacle_position(ObstaclePosition::NONE)
{}

inline void ObstacleAvoidance::begin() {
    Serial.println("Obstacle avoidance initialized");
}

inline bool ObstacleAvoidance::checkForObstacles() {
    distance_sensors.update();
    
    uint16_t front_left = distance_sensors.getFrontLeft();
    uint16_t front_right = distance_sensors.getFrontRight();
    
    bool obstacle_detected = (front_left < Threshold::Distance::OBSTACLE_FAR_MM ||
                             front_right < Threshold::Distance::OBSTACLE_FAR_MM);
    
    if(obstacle_detected) {
        last_obstacle_position = analyzeDistances();
    } else {
        last_obstacle_position = ObstaclePosition::NONE;
    }
    
    return obstacle_detected;
}

inline ObstaclePosition ObstacleAvoidance::getObstaclePosition() {
    return last_obstacle_position;
}

inline ObstaclePosition ObstacleAvoidance::analyzeDistances() {
    uint16_t fl = distance_sensors.getFrontLeft();
    uint16_t fr = distance_sensors.getFrontRight();
    
    bool left_blocked = fl < Threshold::Distance::OBSTACLE_FAR_MM;
    bool right_blocked = fr < Threshold::Distance::OBSTACLE_FAR_MM;
    
    if(left_blocked && right_blocked) {
        return ObstaclePosition::BOTH_SIDES;
    }
    else if(left_blocked) {
        return ObstaclePosition::FRONT_LEFT;
    }
    else if(right_blocked) {
        return ObstaclePosition::FRONT_RIGHT;
    }
    else if(fl < Threshold::Distance::PATH_CLEAR_MM && 
            fr < Threshold::Distance::PATH_CLEAR_MM) {
        return ObstaclePosition::FRONT;
    }
    
    return ObstaclePosition::NONE;
}

inline bool ObstacleAvoidance::isPathClear() {
    distance_sensors.update();
    return distance_sensors.isPathClear();
}

inline uint16_t ObstacleAvoidance::getFrontLeftDistance() const {
    return distance_sensors.getFrontLeft();
}

inline uint16_t ObstacleAvoidance::getFrontRightDistance() const {
    return distance_sensors.getFrontRight();
}

// ============================================
// NAVEGACIÓN ESPECÍFICA DEL CAMPO
// ============================================

inline bool ObstacleAvoidance::detectOpenPathBetweenPools() {
    distance_sensors.update();
    
    // Ambos sensores ToF frontales deben ver distancia larga
    // Esto indica que hay un camino despejado entre las piscinas
    bool open_path = distance_sensors.detectOpenPath();
    
    if(open_path) {
        Serial.println("Open path detected between pools!");
    }
    
    return open_path;
}

inline bool ObstacleAvoidance::navigateThroughPools(uint8_t speed) {
    Serial.println("Navigating through pool area...");
    
    unsigned long start_time = millis();
    unsigned long timeout = 20000; // 20 segundos
    
    while(millis() - start_time < timeout) {
        distance_sensors.update();
        line_sensors.update();
        
        // Verificar que no salimos del campo
        if(line_sensors.detectAnyEdge()) {
            Serial.println("WARNING: Approaching field edge!");
            motors.stopAllMotors(true);
            
            // Retroceder un poco
            movement.moveBackward(100, Speed::SLOW_SPEED);
            delay(200);
            
            return false;
        }
        
        // Buscar camino abierto entre piscinas
        if(detectOpenPathBetweenPools()) {
            Serial.println("Clear path found! Moving forward...");
            movement.moveForward(500, speed); // Avanzar 50cm
            return true;
        }
        
        // Si hay obstáculo, moverse lateralmente para buscar apertura
        ObstaclePosition obs = getObstaclePosition();
        
        if(obs == ObstaclePosition::FRONT_LEFT) {
            // Piscina a la izquierda, moverse a la derecha
            Serial.println("Pool on left, strafing right...");
            movement.strafeRight(100, Speed::SLOW_SPEED);
        }
        else if(obs == ObstaclePosition::FRONT_RIGHT) {
            // Piscina a la derecha, moverse a la izquierda
            Serial.println("Pool on right, strafing left...");
            movement.strafeLeft(100, Speed::SLOW_SPEED);
        }
        else if(obs == ObstaclePosition::BOTH_SIDES) {
            // Bloqueado por ambos lados - retroceder y buscar otra ruta
            Serial.println("Both sides blocked, backing up...");
            movement.moveBackward(200, Speed::SLOW_SPEED);
            movement.strafeRight(150, Speed::SLOW_SPEED);
        }
        
        delay(100);
    }
    
    Serial.println("Timeout navigating through pools!");
    return false;
}

inline bool ObstacleAvoidance::alignWithTree() {
    Serial.println("Aligning with tree...");
    
    // Usar sensores ToF para centrar el robot frente al árbol
    distance_sensors.update();
    
    uint16_t fl = distance_sensors.getFrontLeft();
    uint16_t fr = distance_sensors.getFrontRight();
    
    const uint16_t ALIGNMENT_TOLERANCE = 50; // mm
    int attempts = 0;
    const int MAX_ATTEMPTS = 10;
    
    while(abs(fl - fr) > ALIGNMENT_TOLERANCE && attempts < MAX_ATTEMPTS) {
        distance_sensors.update();
        fl = distance_sensors.getFrontLeft();
        fr = distance_sensors.getFrontRight();
        
        int16_t difference = fl - fr;
        
        Serial.printf("Alignment: FL=%d FR=%d Diff=%d\n", fl, fr, difference);
        
        if(difference > ALIGNMENT_TOLERANCE) {
            // Izquierda más lejos, moverse a la izquierda
            movement.strafeLeft(30, Speed::SLOW_SPEED);
        }
        else if(difference < -ALIGNMENT_TOLERANCE) {
            // Derecha más lejos, moverse a la derecha
            movement.strafeRight(30, Speed::SLOW_SPEED);
        }
        
        delay(200);
        attempts++;
    }
    
    if(attempts >= MAX_ATTEMPTS) {
        Serial.println("Alignment timeout - proceeding anyway");
        return false;
    }
    
    Serial.println("Alignment complete!");
    return true;
}

// ============================================
// EVASIÓN AUTOMÁTICA
// ============================================

inline bool ObstacleAvoidance::avoidObstacle(uint8_t speed) {
    if(!checkForObstacles()) {
        return true; // No hay obstáculos
    }
    
    Serial.println("Avoiding obstacle...");
    ObstaclePosition pos = getObstaclePosition();
    
    switch(pos) {
        case ObstaclePosition::FRONT_LEFT:
            // Moverse a la derecha
            movement.strafeRight(200, speed);
            movement.moveForward(300, speed);
            break;
            
        case ObstaclePosition::FRONT_RIGHT:
            // Moverse a la izquierda
            movement.strafeLeft(200, speed);
            movement.moveForward(300, speed);
            break;
            
        case ObstaclePosition::BOTH_SIDES:
        case ObstaclePosition::FRONT:
            // Retroceder y buscar ruta alternativa
            movement.moveBackward(300, speed);
            movement.strafeRight(250, speed);
            break;
            
        default:
            break;
    }
    
    // Verificar si el camino está despejado ahora
    return waitForClearPath(3000);
}

inline bool ObstacleAvoidance::waitForClearPath(unsigned long timeout_ms) {
    unsigned long start_time = millis();
    
    while(millis() - start_time < timeout_ms) {
        if(isPathClear()) {
            return true;
        }
        delay(100);
    }
    
    return false;
}

// ============================================
// SEGURIDAD
// ============================================

inline bool ObstacleAvoidance::emergencyStop() {
    distance_sensors.update();
    
    // Detección de emergencia: obstáculo muy cerca
    if(distance_sensors.isObstacleNear()) {
        Serial.println("EMERGENCY STOP - Obstacle too close!");
        motors.emergencyStop();
        return true;
    }
    
    return false;
}

inline void ObstacleAvoidance::printObstacleStatus() {
    distance_sensors.update();
    
    Serial.println("\n=== Obstacle Status ===");
    Serial.printf("Front Left:  %d mm\n", distance_sensors.getFrontLeft());
    Serial.printf("Front Right: %d mm\n", distance_sensors.getFrontRight());
    Serial.printf("Path Clear:  %s\n", isPathClear() ? "YES" : "NO");
    
    ObstaclePosition pos = getObstaclePosition();
    Serial.print("Position: ");
    switch(pos) {
        case ObstaclePosition::NONE: Serial.println("NONE"); break;
        case ObstaclePosition::FRONT: Serial.println("FRONT"); break;
        case ObstaclePosition::FRONT_LEFT: Serial.println("FRONT LEFT"); break;
        case ObstaclePosition::FRONT_RIGHT: Serial.println("FRONT RIGHT"); break;
        case ObstaclePosition::BOTH_SIDES: Serial.println("BOTH SIDES"); break;
        default: Serial.println("UNKNOWN"); break;
    }
    Serial.println("=====================\n");
}

#endif // OBSTACLE_AVOIDANCE_H