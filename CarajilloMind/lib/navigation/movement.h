#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include "../drivers/motors.h"
#include "../drivers/imu.h"
#include "../config/constants.h"

// ============================================
// ESTRUCTURAS DE CONTROL
// ============================================

struct PIDController {
    float kp, ki, kd;
    float previous_error;
    float integral;
    float max_output;
    unsigned long last_time;
    
    PIDController(float p, float i, float d, float max_out = 100.0)
        : kp(p), ki(i), kd(d), previous_error(0), integral(0), 
          max_output(max_out), last_time(0) {}
    
    float calculate(float setpoint, float measured_value) {
        unsigned long current_time = millis();
        float dt = (current_time - last_time) / 1000.0;
        
        if(dt <= 0 || last_time == 0) {
            last_time = current_time;
            return 0;
        }
        
        float error = setpoint - measured_value;
        
        // Término proporcional
        float p_term = kp * error;
        
        // Término integral con anti-windup
        integral += error * dt;
        integral = constrain(integral, -max_output/ki, max_output/ki);
        float i_term = ki * integral;
        
        // Término derivativo
        float derivative = (error - previous_error) / dt;
        float d_term = kd * derivative;
        
        // Salida total
        float output = p_term + i_term + d_term;
        output = constrain(output, -max_output, max_output);
        
        // Guardar para siguiente iteración
        previous_error = error;
        last_time = current_time;
        
        return output;
    }
    
    void reset() {
        previous_error = 0;
        integral = 0;
        last_time = 0;
    }
};

// ============================================
// CLASE DE MOVIMIENTO
// ============================================

class Movement {
public:
    Movement(MotorController& motors, IMUController& imu);
    
    // Inicialización
    void begin();
    void update();  // Llamar en loop principal
    
    // Movimientos básicos (con mantenimiento de orientación)
    void moveForward(float distance_mm, uint8_t speed = Speed::CRUISE_SPEED);
    void moveBackward(float distance_mm, uint8_t speed = Speed::CRUISE_SPEED);
    void strafeLeft(float distance_mm, uint8_t speed = Speed::CRUISE_SPEED);
    void strafeRight(float distance_mm, uint8_t speed = Speed::CRUISE_SPEED);
    
    // Rotación
    void rotateTo(float target_heading);
    void rotateRelative(float degrees);  // + = CW, - = CCW
    
    // Movimiento vectorial
    void moveVector(float x_mm, float y_mm, uint8_t speed = Speed::CRUISE_SPEED);
    
    // Control directo (sin mantenimiento de orientación)
    void setVelocity(float vx, float vy, float omega);
    
    // Parada
    void stop(bool brake = true);
    void emergencyStop();
    
    // Estado
    bool isMoving() const;
    bool isAtTarget() const;
    float getDistanceTraveled() const;
    float getCurrentHeading() const;
    
    // Configuración
    void setTargetHeading(float heading);
    void setMaxSpeed(uint8_t speed);
    void enableHeadingHold(bool enable);
    
private:
    MotorController& motors;
    IMUController& imu;
    
    // Control PID
    PIDController heading_pid;
    
    // Estado del movimiento
    float target_heading;
    float start_heading;
    bool heading_hold_enabled;
    uint8_t max_speed;
    
    // Odometría simple
    float distance_traveled;
    unsigned long movement_start_time;
    
    // Helpers
    float calculateHeadingCorrection();
    bool waitForRotationComplete(float target, float tolerance = 2.0, unsigned long timeout_ms = 5000);
    bool waitForDistanceComplete(float target_distance, unsigned long timeout_ms = 10000);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline Movement::Movement(MotorController& motors, IMUController& imu)
    : motors(motors),
      imu(imu),
      heading_pid(PID::Orientation::KP, PID::Orientation::KI, PID::Orientation::KD, 100.0),
      target_heading(0),
      start_heading(0),
      heading_hold_enabled(true),
      max_speed(Speed::CRUISE_SPEED),
      distance_traveled(0),
      movement_start_time(0)
{}

inline void Movement::begin() {
    target_heading = imu.getHeading();
    start_heading = target_heading;
    heading_pid.reset();
    Serial.println("Movement system initialized");
}

inline void Movement::update() {
    // Actualizar control de heading si está habilitado
    if(heading_hold_enabled && motors.isMoving()) {
        float correction = calculateHeadingCorrection();
        
        // La corrección se aplica como rotación
        float current_speed = max_speed * 0.7; // Factor para permitir corrección
        motors.moveOmni(current_speed, 0, correction);
    }
}

inline float Movement::calculateHeadingCorrection() {
    float current_heading = imu.getHeading();
    float heading_error = imu.getRelativeHeading(target_heading);
    
    // PID retorna corrección en rango -100 a +100
    float correction = heading_pid.calculate(0, heading_error);
    
    return correction;
}

// ============================================
// MOVIMIENTOS BÁSICOS
// ============================================

inline void Movement::moveForward(float distance_mm, uint8_t speed) {
    Serial.printf("Moving forward %.1f mm at speed %d\n", distance_mm, speed);
    
    target_heading = imu.getHeading();
    heading_pid.reset();
    distance_traveled = 0;
    movement_start_time = millis();
    
    // Tiempo estimado basado en velocidad
    float estimated_time_ms = (distance_mm / Speed::MAX_LINEAR_SPEED_MMS) * 1000.0;
    estimated_time_ms *= (255.0 / speed); // Ajustar por velocidad real
    
    motors.moveForward(speed);
    
    // Esperar a completar distancia (simplificado con tiempo)
    delay(estimated_time_ms);
    
    motors.stopAllMotors(true);
    delay(100); // Estabilizar
}

inline void Movement::moveBackward(float distance_mm, uint8_t speed) {
    Serial.printf("Moving backward %.1f mm at speed %d\n", distance_mm, speed);
    
    target_heading = imu.getHeading();
    heading_pid.reset();
    
    float estimated_time_ms = (distance_mm / Speed::MAX_LINEAR_SPEED_MMS) * 1000.0;
    estimated_time_ms *= (255.0 / speed);
    
    motors.moveBackward(speed);
    delay(estimated_time_ms);
    motors.stopAllMotors(true);
    delay(100);
}

inline void Movement::strafeLeft(float distance_mm, uint8_t speed) {
    Serial.printf("Strafing left %.1f mm at speed %d\n", distance_mm, speed);
    
    target_heading = imu.getHeading();
    heading_pid.reset();
    
    float estimated_time_ms = (distance_mm / Speed::MAX_LINEAR_SPEED_MMS) * 1000.0;
    estimated_time_ms *= (255.0 / speed);
    
    motors.strafeLeft(speed);
    delay(estimated_time_ms);
    motors.stopAllMotors(true);
    delay(100);
}

inline void Movement::strafeRight(float distance_mm, uint8_t speed) {
    Serial.printf("Strafing right %.1f mm at speed %d\n", distance_mm, speed);
    
    target_heading = imu.getHeading();
    heading_pid.reset();
    
    float estimated_time_ms = (distance_mm / Speed::MAX_LINEAR_SPEED_MMS) * 1000.0;
    estimated_time_ms *= (255.0 / speed);
    
    motors.strafeRight(speed);
    delay(estimated_time_ms);
    motors.stopAllMotors(true);
    delay(100);
}

// ============================================
// ROTACIÓN
// ============================================

inline void Movement::rotateTo(float target) {
    Serial.printf("Rotating to %.1f°\n", target);
    
    target_heading = target;
    float current_heading = imu.getHeading();
    float error = imu.getRelativeHeading(target);
    
    // Determinar dirección de rotación más corta
    uint8_t rotation_speed = Speed::TURN_SPEED;
    
    PIDController rotation_pid(3.0, 0.0, 1.0, rotation_speed);
    
    unsigned long start_time = millis();
    unsigned long timeout = 5000;
    
    while(abs(error) > PID::Orientation::TOLERANCE_DEG) {
        if(millis() - start_time > timeout) {
            Serial.println("Rotation timeout!");
            break;
        }
        
        imu.update();
        current_heading = imu.getHeading();
        error = imu.getRelativeHeading(target);
        
        float correction = rotation_pid.calculate(0, error);
        
        if(abs(correction) > 10) {
            motors.moveOmni(0, 0, correction);
        } else {
            break; // Casi en posición
        }
        
        delay(20);
    }
    
    motors.stopAllMotors(true);
    delay(200); // Estabilizar
    
    Serial.printf("Rotation complete. Final heading: %.1f°\n", imu.getHeading());
}

inline void Movement::rotateRelative(float degrees) {
    float current = imu.getHeading();
    float target = current + degrees;
    
    // Normalizar
    while(target < 0) target += 360;
    while(target >= 360) target -= 360;
    
    rotateTo(target);
}

inline void Movement::moveVector(float x_mm, float y_mm, uint8_t speed) {
    Serial.printf("Moving vector: x=%.1f y=%.1f mm\n", x_mm, y_mm);
    
    // Calcular distancia y ángulo
    float distance = sqrt(x_mm * x_mm + y_mm * y_mm);
    float angle = atan2(y_mm, x_mm) * 180.0 / PI;
    
    // Rotar hacia el ángulo objetivo
    rotateTo(angle);
    
    // Moverse hacia adelante
    moveForward(distance, speed);
}

// ============================================
// CONTROL DIRECTO
// ============================================

inline void Movement::setVelocity(float vx, float vy, float omega) {
    heading_hold_enabled = false;
    motors.moveOmni(vx, vy, omega);
}

inline void Movement::stop(bool brake) {
    motors.stopAllMotors(brake);
    heading_pid.reset();
}

inline void Movement::emergencyStop() {
    motors.emergencyStop();
    heading_pid.reset();
}

// ============================================
// ESTADO
// ============================================

inline bool Movement::isMoving() const {
    return motors.isMoving();
}

inline bool Movement::isAtTarget() const {
    float heading_error = abs(imu.getRelativeHeading(target_heading));
    return heading_error < PID::Orientation::TOLERANCE_DEG;
}

inline float Movement::getDistanceTraveled() const {
    return distance_traveled;
}

inline float Movement::getCurrentHeading() const {
    return imu.getHeading();
}

// ============================================
// CONFIGURACIÓN
// ============================================

inline void Movement::setTargetHeading(float heading) {
    target_heading = heading;
    heading_pid.reset();
}

inline void Movement::setMaxSpeed(uint8_t speed) {
    max_speed = constrain(speed, 0, 255);
}

inline void Movement::enableHeadingHold(bool enable) {
    heading_hold_enabled = enable;
    if(enable) {
        target_heading = imu.getHeading();
        heading_pid.reset();
    }
}

#endif // MOVEMENT_H