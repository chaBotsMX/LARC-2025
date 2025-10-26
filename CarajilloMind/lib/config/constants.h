#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ============================================
// DIMENSIONES DEL ROBOT
// ============================================
namespace Robot {
    constexpr float WHEEL_DIAMETER_MM = 60.0;
    constexpr float WHEEL_BASE_MM = 200.0;      // Distancia entre ruedas
    constexpr float ROBOT_WIDTH_MM = 400.0;
    constexpr float ROBOT_LENGTH_MM = 400.0;
    constexpr float MAX_SIZE_MM = 450.0;        // Límite del cubo inicial
}

// ============================================
// DIMENSIONES DEL CAMPO (según reglas LARC)
// ============================================
namespace Field {
    // Dimensiones totales
    constexpr float TOTAL_WIDTH_MM = 3600.0;
    constexpr float TOTAL_LENGTH_MM = 3000.0;
    
    // Zona de cultivo (cada sector)
    constexpr float TREE_SECTOR_WIDTH_MM = 1174.5;
    constexpr float TREE_SECTOR_LENGTH_MM = 412.0;
    
    // Zona de procesamiento (cada sector)
    constexpr float FACILITY_SECTOR_WIDTH_MM = 712.0;
    constexpr float FACILITY_SECTOR_LENGTH_MM = 697.0;
    
    // Piscinas
    constexpr float POOL_LENGTH_MM = 720.0;
    constexpr float POOL_WIDTH_MM = 200.0;
    constexpr float POOL_HEIGHT_MM = 90.0;
    
    // Líneas
    constexpr float LINE_WIDTH_MM = 19.0;
    
    // Contenedores
    constexpr float CONTAINER_SIZE_MM = 450.0;
    constexpr float CONTAINER_HEIGHT_MM = 90.0;
}

// ============================================
// ÁRBOLES Y GRANOS
// ============================================
namespace Tree {
    constexpr int NUM_TREES = 3;
    constexpr int NUM_LEVELS = 3;
    constexpr int BEANS_PER_LEVEL[] = {5, 6, 5}; // Bajo, medio, alto
    constexpr int TOTAL_BEANS_PER_TREE = 16;
    
    // Alturas de los niveles (desde el suelo)
    constexpr float LEVEL_HEIGHT_MM[] = {150.0, 250.0, 350.0};
    
    // Posición del árbol en el sector
    constexpr float TREE_OFFSET_FROM_LINE_MM = 100.0;
    
    // Dimensiones del grano
    constexpr float BEAN_DIAMETER_MM = 42.0;
    constexpr float BEAN_WEIGHT_G = 12.0;
    constexpr float BEAN_WEIGHT_TOLERANCE_G = 4.0;
    
    // Holder
    constexpr float HOLDER_HOLE_DIAMETER_MM = 25.0;
}

// ============================================
// VELOCIDADES Y TIEMPOS
// ============================================
namespace Speed {
    // Velocidades del motor (PWM 0-255)
    constexpr uint8_t MAX_SPEED = 200;
    constexpr uint8_t CRUISE_SPEED = 150;
    constexpr uint8_t SLOW_SPEED = 80;
    constexpr uint8_t COLLECTION_SPEED = 60;
    constexpr uint8_t TURN_SPEED = 100;
    
    // Velocidades en mm/s (para cálculos)
    constexpr float MAX_LINEAR_SPEED_MMS = 500.0;
    constexpr float MAX_ANGULAR_SPEED_DEGS = 180.0;
    
    // Tiempos
    constexpr unsigned long MAX_ROUND_TIME_MS = 480000; // 8 minutos
    constexpr unsigned long COLLECTION_TIMEOUT_MS = 5000;
    constexpr unsigned long SERVO_DELAY_MS = 500;
}

// ============================================
// PARÁMETROS PID
// ============================================
namespace PID {
    // Line Following
    namespace LineFollower {
        constexpr float KP = 2.0;
        constexpr float KI = 0.1;
        constexpr float KD = 1.5;
        constexpr float MAX_CORRECTION = 100.0;
    }
    
    // Motor Speed Control
    namespace MotorSpeed {
        constexpr float KP = 1.5;
        constexpr float KI = 0.05;
        constexpr float KD = 0.5;
    }
    
    // Orientation Hold (IMU)
    namespace Orientation {
        constexpr float KP = 3.0;
        constexpr float KI = 0.0;
        constexpr float KD = 1.0;
        constexpr float TOLERANCE_DEG = 2.0;
    }
}

// ============================================
// UMBRALES DE SENSORES
// ============================================
namespace Threshold {
    // Sensores de línea (TCRT5000)
    namespace Line {
        constexpr uint16_t BLACK_LINE = 800;     // Por encima es negro
        constexpr uint16_t WHITE_SURFACE = 200;  // Por debajo es blanco
        constexpr uint16_t HYSTERESIS = 50;
    }
    
    // Sensores ToF (VL53L0X) en mm
    namespace Distance {
        constexpr uint16_t OBSTACLE_NEAR_MM = 100;
        constexpr uint16_t OBSTACLE_FAR_MM = 300;
        constexpr uint16_t PATH_CLEAR_MM = 800;
        constexpr uint16_t POOL_DETECTION_MM = 200;
        constexpr uint16_t MAX_RANGE_MM = 2000;
        constexpr uint16_t INVALID_READING = 8190; // Error del sensor
    }
    
    // Sensor de color (TCS34725)
    namespace Color {
        // Umbrales RGB para clasificación
        // Estos valores deben calibrarse en campo
        struct ColorRange {
            uint16_t r_min, r_max;
            uint16_t g_min, g_max;
            uint16_t b_min, b_max;
        };
        
        // Valores aproximados - CALIBRAR EN CAMPO
        constexpr ColorRange RED = {200, 255, 0, 100, 0, 100};
        constexpr ColorRange ORANGE = {200, 255, 100, 200, 0, 100};
        constexpr ColorRange YELLOW = {200, 255, 180, 255, 0, 150};
        constexpr ColorRange GREEN = {0, 150, 150, 255, 0, 150};
        constexpr ColorRange BLUE = {0, 100, 0, 150, 150, 255};
        constexpr ColorRange BLACK = {0, 80, 0, 80, 0, 80};
        
        // Umbral de confianza mínima
        constexpr uint16_t MIN_BRIGHTNESS = 50;
        constexpr uint16_t MAX_BRIGHTNESS = 60000;
    }
}

// ============================================
// POSICIONES DE SERVOS
// ============================================
namespace ServoPos {
    // Brazo de levantamiento (MG995)
    namespace ArmLift {
        constexpr uint16_t RETRACTED = 1000;      // Posición guardada
        constexpr uint16_t LEVEL_1 = 1500;        // Nivel bajo
        constexpr uint16_t LEVEL_2 = 2000;        // Nivel medio
        constexpr uint16_t LEVEL_3 = 2400;        // Nivel alto
    }
    
    // Extensión del brazo (SG90)
    namespace ArmExtend {
        constexpr uint16_t RETRACTED = 1000;
        constexpr uint16_t EXTENDED = 2000;
    }
    
    // Pinza de agarre (SG90)
    namespace Gripper {
        constexpr uint16_t OPEN = 1000;
        constexpr uint16_t CLOSED = 2000;
    }
    
    // Compuerta del carrusel (SG90)
    namespace CarouselGate {
        constexpr uint16_t CLOSED = 1000;
        constexpr uint16_t RIPE = 1500;           // Hacia contenedor rojo
        constexpr uint16_t OVERRIPE = 2000;       // Hacia contenedor azul
    }
    
    // Compuerta del elevador (SG90)
    namespace ElevatorGate {
        constexpr uint16_t CLOSED = 1000;
        constexpr uint16_t OPEN = 2000;
    }
}

// ============================================
// CONFIGURACIÓN DEL MOTOR STEPPER
// ============================================
namespace Stepper {
    constexpr uint16_t STEPS_PER_REVOLUTION = 200; // Motor típico 1.8°
    constexpr uint8_t MICROSTEPS = 16;             // DRV8834 configuración
    constexpr uint16_t TOTAL_STEPS = STEPS_PER_REVOLUTION * MICROSTEPS;
    constexpr uint16_t MAX_SPEED_STEPS_PER_SEC = 1000;
    constexpr uint16_t ACCELERATION_STEPS_PER_SEC2 = 500;
}

// ============================================
// PUNTUACIÓN (según reglas LARC)
// ============================================
namespace Scoring {
    constexpr int8_t RIPE_COLLECTED = 3;
    constexpr int8_t OVERRIPE_COLLECTED = 1;
    constexpr int8_t RIPE_DEPOSITED = 7;        // +3 + 4 bonus
    constexpr int8_t OVERRIPE_DEPOSITED = 5;    // +1 + 4 bonus
    
    constexpr int8_t PENALTY_ABANDONED = -7;
    constexpr int8_t PENALTY_GREEN = -7;
    constexpr int8_t PENALTY_WRONG_RIPE = -7;
    constexpr int8_t PENALTY_WRONG_OVERRIPE = -5;
    
    constexpr uint8_t MAX_RESTARTS = 2;
}

// ============================================
// CONFIGURACIÓN DEL SISTEMA
// ============================================
namespace System {
    constexpr unsigned long SERIAL_BAUD = 115200;
    constexpr unsigned long I2C_CLOCK = 400000;   // 400kHz Fast Mode
    constexpr unsigned long LOOP_RATE_MS = 10;     // 100Hz
    constexpr unsigned long SENSOR_READ_RATE_MS = 20; // 50Hz
    
    // Timeouts
    constexpr unsigned long I2C_TIMEOUT_MS = 100;
    constexpr unsigned long SENSOR_TIMEOUT_MS = 500;
    constexpr unsigned long MOVEMENT_TIMEOUT_MS = 10000;
    
    // Número máximo de reintentos
    constexpr uint8_t MAX_SENSOR_RETRIES = 3;
    constexpr uint8_t MAX_MOVEMENT_RETRIES = 2;
}

// ============================================
// MODOS DE OPERACIÓN
// ============================================
enum class RobotMode {
    IDLE,
    CALIBRATION,
    AUTONOMOUS,
    MANUAL,
    TEST,
    EMERGENCY_STOP
};

enum class MissionState {
    INIT,
    NAVIGATE_TO_TREE_1,
    HARVEST_TREE_1,
    NAVIGATE_TO_TREE_2,
    HARVEST_TREE_2,
    NAVIGATE_TO_TREE_3,
    HARVEST_TREE_3,
    RETURN_TO_FACILITY,
    DEPOSIT_BEANS,
    MISSION_COMPLETE,
    ERROR
};

// ============================================
// TIPOS DE GRANOS
// ============================================
enum class BeanType {
    NONE,
    GREEN,          // No recolectar
    RIPE,           // Rojo, naranja, amarillo -> Contenedor rojo
    OVERRIPE,       // Azul, negro -> Contenedor azul
    UNKNOWN
};

// ============================================
// DEBUG
// ============================================
#ifdef DEBUG_MODE
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(fmt, ...)
#endif

#endif // CONSTANTS_H