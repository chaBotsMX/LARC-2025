#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

// ============================================
// ESTRUCTURA DE CALIBRACIÓN
// ============================================

struct CalibrationData {
    // Sensores de línea
    struct {
        uint16_t black_threshold[8];
        uint16_t white_threshold[8];
        bool inverted[8];  // Si el sensor está invertido
    } line_sensors;
    
    // IMU
    struct {
        float heading_offset;      // Offset del heading inicial
        float pitch_offset;
        float roll_offset;
        bool is_calibrated;
    } imu;
    
    // Motores
    struct {
        float speed_correction[4]; // Corrección por motor
        bool reversed[4];          // Si el motor está invertido
    } motors;
    
    // Sensores de color
    struct {
        // Valores RGB calibrados para cada color
        uint16_t red_r, red_g, red_b;
        uint16_t orange_r, orange_g, orange_b;
        uint16_t yellow_r, yellow_g, yellow_b;
        uint16_t green_r, green_g, green_b;
        uint16_t blue_r, blue_g, blue_b;
        uint16_t black_r, black_g, black_b;
        
        // Tolerancia para cada color
        uint16_t tolerance;
    } color_sensors;
    
    // Servos
    struct {
        uint16_t arm_lift_min;
        uint16_t arm_lift_max;
        uint16_t arm_extend_min;
        uint16_t arm_extend_max;
        uint16_t gripper_open;
        uint16_t gripper_closed;
    } servos;
    
    // Timestamp de última calibración
    unsigned long last_calibration_time;
    bool is_valid;
};

// ============================================
// CLASE DE GESTIÓN DE CALIBRACIÓN
// ============================================

class CalibrationManager {
public:
    CalibrationManager();
    
    // Cargar/Guardar calibración
    bool loadCalibration();
    bool saveCalibration();
    void resetToDefaults();
    
    // Calibración interactiva
    void calibrateLineSensors();
    void calibrateIMU();
    void calibrateMotors();
    void calibrateColorSensors();
    void calibrateServos();
    
    // Getters
    CalibrationData& getData() { return data; }
    bool isValid() const { return data.is_valid; }
    
    // Imprimir valores actuales
    void printCalibration();
    
private:
    CalibrationData data;
    
    // Helpers
    void waitForUserInput();
    uint16_t readAverageLineSensor(uint8_t pin, int samples = 100);
    void saveToEEPROM();
    bool loadFromEEPROM();
};

// ============================================
// VALORES POR DEFECTO
// ============================================

namespace DefaultCalibration {
    // Sensores de línea (valores aproximados)
    constexpr uint16_t LINE_BLACK_DEFAULT = 800;
    constexpr uint16_t LINE_WHITE_DEFAULT = 200;
    
    // IMU
    constexpr float IMU_HEADING_DEFAULT = 0.0;
    
    // Motores (sin corrección por defecto)
    constexpr float MOTOR_SPEED_CORRECTION_DEFAULT = 1.0;
    
    // Servos (valores medios seguros)
    constexpr uint16_t SERVO_MIN_DEFAULT = 1000;
    constexpr uint16_t SERVO_MAX_DEFAULT = 2000;
    constexpr uint16_t SERVO_MID_DEFAULT = 1500;
    
    // Color (valores RGB aproximados - DEBEN calibrarse)
    constexpr uint16_t COLOR_TOLERANCE_DEFAULT = 50;
}

// ============================================
// IMPLEMENTACIÓN INLINE DE FUNCIONES SIMPLES
// ============================================

inline CalibrationManager::CalibrationManager() {
    resetToDefaults();
}

inline void CalibrationManager::resetToDefaults() {
    // Line sensors
    for(int i = 0; i < 8; i++) {
        data.line_sensors.black_threshold[i] = DefaultCalibration::LINE_BLACK_DEFAULT;
        data.line_sensors.white_threshold[i] = DefaultCalibration::LINE_WHITE_DEFAULT;
        data.line_sensors.inverted[i] = false;
    }
    
    // IMU
    data.imu.heading_offset = DefaultCalibration::IMU_HEADING_DEFAULT;
    data.imu.pitch_offset = 0.0;
    data.imu.roll_offset = 0.0;
    data.imu.is_calibrated = false;
    
    // Motors
    for(int i = 0; i < 4; i++) {
        data.motors.speed_correction[i] = DefaultCalibration::MOTOR_SPEED_CORRECTION_DEFAULT;
        data.motors.reversed[i] = false;
    }
    
    // Color sensors - valores genéricos
    data.color_sensors.red_r = 255; data.color_sensors.red_g = 0; data.color_sensors.red_b = 0;
    data.color_sensors.orange_r = 255; data.color_sensors.orange_g = 165; data.color_sensors.orange_b = 0;
    data.color_sensors.yellow_r = 255; data.color_sensors.yellow_g = 255; data.color_sensors.yellow_b = 0;
    data.color_sensors.green_r = 0; data.color_sensors.green_g = 255; data.color_sensors.green_b = 0;
    data.color_sensors.blue_r = 0; data.color_sensors.blue_g = 0; data.color_sensors.blue_b = 255;
    data.color_sensors.black_r = 0; data.color_sensors.black_g = 0; data.color_sensors.black_b = 0;
    data.color_sensors.tolerance = DefaultCalibration::COLOR_TOLERANCE_DEFAULT;
    
    // Servos
    data.servos.arm_lift_min = DefaultCalibration::SERVO_MIN_DEFAULT;
    data.servos.arm_lift_max = DefaultCalibration::SERVO_MAX_DEFAULT;
    data.servos.arm_extend_min = DefaultCalibration::SERVO_MIN_DEFAULT;
    data.servos.arm_extend_max = DefaultCalibration::SERVO_MAX_DEFAULT;
    data.servos.gripper_open = DefaultCalibration::SERVO_MIN_DEFAULT;
    data.servos.gripper_closed = DefaultCalibration::SERVO_MAX_DEFAULT;
    
    data.last_calibration_time = 0;
    data.is_valid = false;
}

inline void CalibrationManager::printCalibration() {
    Serial.println("\n=== CALIBRATION DATA ===");
    
    Serial.println("\n--- Line Sensors ---");
    for(int i = 0; i < 8; i++) {
        Serial.printf("Sensor %d: Black=%d, White=%d, Inverted=%s\n", 
            i, 
            data.line_sensors.black_threshold[i],
            data.line_sensors.white_threshold[i],
            data.line_sensors.inverted[i] ? "Yes" : "No"
        );
    }
    
    Serial.println("\n--- IMU ---");
    Serial.printf("Heading Offset: %.2f°\n", data.imu.heading_offset);
    Serial.printf("Calibrated: %s\n", data.imu.is_calibrated ? "Yes" : "No");
    
    Serial.println("\n--- Motors ---");
    for(int i = 0; i < 4; i++) {
        Serial.printf("Motor %d: Correction=%.2f, Reversed=%s\n",
            i,
            data.motors.speed_correction[i],
            data.motors.reversed[i] ? "Yes" : "No"
        );
    }
    
    Serial.println("\n--- Color Sensors ---");
    Serial.printf("Red: R=%d G=%d B=%d\n", data.color_sensors.red_r, data.color_sensors.red_g, data.color_sensors.red_b);
    Serial.printf("Orange: R=%d G=%d B=%d\n", data.color_sensors.orange_r, data.color_sensors.orange_g, data.color_sensors.orange_b);
    Serial.printf("Yellow: R=%d G=%d B=%d\n", data.color_sensors.yellow_r, data.color_sensors.yellow_g, data.color_sensors.yellow_b);
    Serial.printf("Green: R=%d G=%d B=%d\n", data.color_sensors.green_r, data.color_sensors.green_g, data.color_sensors.green_b);
    Serial.printf("Blue: R=%d G=%d B=%d\n", data.color_sensors.blue_r, data.color_sensors.blue_g, data.color_sensors.blue_b);
    Serial.printf("Black: R=%d G=%d B=%d\n", data.color_sensors.black_r, data.color_sensors.black_g, data.color_sensors.black_b);
    Serial.printf("Tolerance: %d\n", data.color_sensors.tolerance);
    
    Serial.println("\n--- Servos ---");
    Serial.printf("Arm Lift: Min=%d Max=%d\n", data.servos.arm_lift_min, data.servos.arm_lift_max);
    Serial.printf("Arm Extend: Min=%d Max=%d\n", data.servos.arm_extend_min, data.servos.arm_extend_max);
    Serial.printf("Gripper: Open=%d Closed=%d\n", data.servos.gripper_open, data.servos.gripper_closed);
    
    Serial.println("\n========================\n");
}

#endif // CALIBRATION_H