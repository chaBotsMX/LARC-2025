#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include "../Config/constants.h"

// ============================================
// ESTRUCTURA DE DATOS DE ORIENTACIÓN
// ============================================

struct Orientation {
    float heading;      // 0-360° (yaw)
    float pitch;        // -180 a +180°
    float roll;         // -180 a +180°
    float heading_rate; // deg/s
    bool is_valid;
    
    Orientation() : heading(0), pitch(0), roll(0), heading_rate(0), is_valid(false) {}
};

// ============================================
// CLASE DE CONTROL DE IMU
// ============================================

class IMUController {
public:
    IMUController();
    
    // Inicialización
    bool begin(TwoWire& wire = Wire);
    void calibrate();
    bool isCalibrated() const;
    
    // Actualización
    void update();
    
    // Lecturas de orientación
    Orientation getOrientation() const;
    float getHeading() const;          // 0-360°
    float getPitch() const;
    float getRoll() const;
    float getHeadingRate() const;      // Velocidad angular en Z
    
    // Utilidades de navegación
    float getRelativeHeading(float target_heading) const;  // -180 a +180°
    bool isHeadingStable(float tolerance = 2.0) const;
    void setHeadingOffset(float offset);
    void resetHeading();
    
    // Estado del sensor
    uint8_t getCalibrationStatus();
    bool isSystemCalibrated() const;
    void printCalibrationStatus();
    void printOrientation();
    
private:
    Adafruit_BNO055 bno;
    Orientation current_orientation;
    Orientation previous_orientation;
    
    float heading_offset;
    unsigned long last_update_time;
    bool initialized;
    bool calibrated;
    
    // Filtrado
    static constexpr int FILTER_SIZE = 5;
    float heading_buffer[FILTER_SIZE];
    int buffer_index;
    
    // Helpers
    float normalizeAngle(float angle);
    float getFilteredHeading();
    void updateHeadingRate();
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline IMUController::IMUController() 
    : bno(55, 0x28, &Wire),
      heading_offset(0.0),
      last_update_time(0),
      initialized(false),
      calibrated(false),
      buffer_index(0)
{
    for(int i = 0; i < FILTER_SIZE; i++) {
        heading_buffer[i] = 0.0;
    }
}

inline bool IMUController::begin(TwoWire& wire) {
    Serial.println("Initializing BNO055...");
    
    if(!bno.begin()) {
        Serial.println("ERROR: BNO055 not detected!");
        return false;
    }
    
    delay(1000);
    bno.setExtCrystalUse(true);
    
    initialized = true;
    last_update_time = millis();
    
    Serial.println("BNO055 initialized successfully");
    return true;
}

inline void IMUController::calibrate() {
    Serial.println("\n=== IMU CALIBRATION ===");
    Serial.println("Place robot on flat surface and rotate slowly...");
    Serial.println("Calibration status (Sys Gyro Acc Mag): ");
    
    uint8_t sys, gyro, accel, mag = 0;
    
    while(!isSystemCalibrated()) {
        bno.getCalibration(&sys, &gyro, &accel, &mag);
        
        Serial.printf("\r%d %d %d %d  ", sys, gyro, accel, mag);
        
        delay(100);
    }
    
    Serial.println("\n\nCalibration complete!");
    calibrated = true;
    
    // Guardar offset del heading actual
    sensors_event_t event;
    bno.getEvent(&event);
    heading_offset = event.orientation.x;
    
    Serial.printf("Heading offset set to: %.2f°\n", heading_offset);
}

inline bool IMUController::isCalibrated() const {
    return calibrated;
}

inline bool IMUController::isSystemCalibrated() const {
    uint8_t sys, gyro, accel, mag = 0;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    
    // Sistema calibrado cuando sys=3 y gyro=3
    return (sys >= 3 && gyro >= 3);
}

inline void IMUController::update() {
    if(!initialized) return;
    
    // Guardar orientación anterior
    previous_orientation = current_orientation;
    
    // Leer nueva orientación
    sensors_event_t event;
    if(bno.getEvent(&event)) {
        // Heading (0-360°)
        float raw_heading = event.orientation.x;
        current_orientation.heading = normalizeAngle(raw_heading - heading_offset);
        
        // Pitch y Roll
        current_orientation.pitch = event.orientation.y;
        current_orientation.roll = event.orientation.z;
        
        // Agregar al buffer de filtrado
        heading_buffer[buffer_index] = current_orientation.heading;
        buffer_index = (buffer_index + 1) % FILTER_SIZE;
        
        // Calcular velocidad angular
        updateHeadingRate();
        
        current_orientation.is_valid = true;
    } else {
        current_orientation.is_valid = false;
        Serial.println("Warning: IMU reading failed");
    }
    
    last_update_time = millis();
}

inline void IMUController::updateHeadingRate() {
    unsigned long current_time = millis();
    float dt = (current_time - last_update_time) / 1000.0; // segundos
    
    if(dt > 0 && previous_orientation.is_valid) {
        float heading_diff = current_orientation.heading - previous_orientation.heading;
        
        // Manejar cruce de 0/360°
        if(heading_diff > 180) heading_diff -= 360;
        if(heading_diff < -180) heading_diff += 360;
        
        current_orientation.heading_rate = heading_diff / dt;
    } else {
        current_orientation.heading_rate = 0.0;
    }
}

inline Orientation IMUController::getOrientation() const {
    return current_orientation;
}

inline float IMUController::getHeading() const {
    return getFilteredHeading();
}

inline float IMUController::getPitch() const {
    return current_orientation.pitch;
}

inline float IMUController::getRoll() const {
    return current_orientation.roll;
}

inline float IMUController::getHeadingRate() const {
    return current_orientation.heading_rate;
}

inline float IMUController::getFilteredHeading() {
    // Media móvil simple
    float sum = 0;
    int count = 0;
    
    for(int i = 0; i < FILTER_SIZE; i++) {
        if(heading_buffer[i] != 0 || i == buffer_index) {
            sum += heading_buffer[i];
            count++;
        }
    }
    
    return count > 0 ? sum / count : current_orientation.heading;
}

inline float IMUController::getRelativeHeading(float target_heading) const {
    float current = getHeading();
    float diff = target_heading - current;
    
    // Normalizar a rango -180 a +180
    while(diff > 180) diff -= 360;
    while(diff < -180) diff += 360;
    
    return diff;
}

inline bool IMUController::isHeadingStable(float tolerance) const {
    return abs(current_orientation.heading_rate) < tolerance;
}

inline void IMUController::setHeadingOffset(float offset) {
    heading_offset = offset;
    Serial.printf("Heading offset updated to: %.2f°\n", heading_offset);
}

inline void IMUController::resetHeading() {
    sensors_event_t event;
    if(bno.getEvent(&event)) {
        heading_offset = event.orientation.x;
        Serial.println("Heading reset to current direction as 0°");
    }
}

inline float IMUController::normalizeAngle(float angle) {
    while(angle < 0) angle += 360;
    while(angle >= 360) angle -= 360;
    return angle;
}

inline uint8_t IMUController::getCalibrationStatus() {
    uint8_t sys, gyro, accel, mag = 0;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    return sys;
}

inline void IMUController::printCalibrationStatus() {
    uint8_t sys, gyro, accel, mag = 0;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    
    Serial.println("\n=== IMU Calibration Status ===");
    Serial.printf("System: %d/3\n", sys);
    Serial.printf("Gyro:   %d/3\n", gyro);
    Serial.printf("Accel:  %d/3\n", accel);
    Serial.printf("Mag:    %d/3\n", mag);
    Serial.println("============================\n");
}

inline void IMUController::printOrientation() {
    if(!current_orientation.is_valid) {
        Serial.println("IMU: Invalid reading");
        return;
    }
    
    Serial.printf("IMU: Heading=%.1f° Pitch=%.1f° Roll=%.1f° Rate=%.1f°/s\n",
        getHeading(),
        current_orientation.pitch,
        current_orientation.roll,
        current_orientation.heading_rate
    );
}

#endif // IMU_H