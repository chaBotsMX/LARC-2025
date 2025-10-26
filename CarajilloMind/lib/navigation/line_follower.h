#ifndef LINE_FOLLOWER_H
#define LINE_FOLLOWER_H

#include <Arduino.h>
#include "../drivers/motors.h"
#include "../drivers/sensors.h"
#include "../config/constants.h"

// ============================================
// ENUMERACIONES
// ============================================

enum class LineFollowState {
    ON_LINE,
    LINE_LOST,
    EDGE_DETECTED,
    INTERSECTION_DETECTED
};

// ============================================
// CLASE DE SEGUIDOR DE LÍNEA
// ============================================

class LineFollower {
public:
    LineFollower(MotorController& motors, LineSensors& sensors);
    
    // Inicialización
    void begin();
    
    // Seguimiento de línea
    void followLine(uint8_t base_speed = Speed::CRUISE_SPEED);
    void followLineDistance(float distance_mm, uint8_t base_speed = Speed::CRUISE_SPEED);
    void followLineUntilIntersection(uint8_t base_speed = Speed::CRUISE_SPEED);
    
    // Detección de condiciones especiales
    bool detectIntersection();
    bool detectLineEnd();
    
    // Control
    void stop();
    void setPID(float kp, float ki, float kd);
    
    // Estado
    LineFollowState getState() const;
    int16_t getLineError() const;
    bool isOnLine() const;
    
private:
    MotorController& motors;
    LineSensors& sensors;
    
    // Control PID
    float kp, ki, kd;
    float previous_error;
    float integral;
    unsigned long last_time;
    
    // Estado
    LineFollowState current_state;
    int16_t line_error;
    
    // Helpers
    float calculatePID(float error);
    void updateState();
    void applyCorrection(float correction, uint8_t base_speed);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline LineFollower::LineFollower(MotorController& motors, LineSensors& sensors)
    : motors(motors),
      sensors(sensors),
      kp(PID::LineFollower::KP),
      ki(PID::LineFollower::KI),
      kd(PID::LineFollower::KD),
      previous_error(0),
      integral(0),
      last_time(0),
      current_state(LineFollowState::LINE_LOST),
      line_error(0)
{}

inline void LineFollower::begin() {
    previous_error = 0;
    integral = 0;
    last_time = millis();
    Serial.println("Line follower initialized");
}

inline void LineFollower::followLine(uint8_t base_speed) {
    sensors.update();
    updateState();
    
    if(current_state == LineFollowState::EDGE_DETECTED) {
        // Detenerse si detecta borde del campo
        motors.stopAllMotors(true);
        Serial.println("Edge detected! Stopping.");
        return;
    }
    
    // Obtener posición de la línea (-100 a +100)
    line_error = sensors.getLinePosition();
    
    // Calcular corrección PID
    float correction = calculatePID(line_error);
    
    // Aplicar corrección
    applyCorrection(correction, base_speed);
}

inline void LineFollower::followLineDistance(float distance_mm, uint8_t base_speed) {
    Serial.printf("Following line for %.1f mm\n", distance_mm);
    
    // Estimación simple basada en tiempo
    float estimated_time_ms = (distance_mm / Speed::MAX_LINEAR_SPEED_MMS) * 1000.0;
    estimated_time_ms *= (255.0 / base_speed); // Ajustar por velocidad
    
    unsigned long start_time = millis();
    unsigned long target_time = start_time + estimated_time_ms;
    
    while(millis() < target_time) {
        sensors.update();
        
        // Verificar bordes
        if(sensors.detectAnyEdge()) {
            Serial.println("Edge detected during line following!");
            motors.stopAllMotors(true);
            return;
        }
        
        followLine(base_speed);
        delay(10);
    }
    
    motors.stopAllMotors(true);
    Serial.println("Distance complete");
}

inline void LineFollower::followLineUntilIntersection(uint8_t base_speed) {
    Serial.println("Following line until intersection...");
    
    unsigned long start_time = millis();
    unsigned long timeout = 30000; // 30 segundos máximo
    
    while(millis() - start_time < timeout) {
        sensors.update();
        
        if(detectIntersection()) {
            Serial.println("Intersection detected!");
            motors.stopAllMotors(true);
            delay(200);
            return;
        }
        
        if(sensors.detectAnyEdge()) {
            Serial.println("Edge detected!");
            motors.stopAllMotors(true);
            return;
        }
        
        followLine(base_speed);
        delay(10);
    }
    
    Serial.println("Timeout waiting for intersection");
    motors.stopAllMotors(true);
}

inline bool LineFollower::detectIntersection() {
    // Intersección: múltiples sensores detectan línea simultáneamente
    sensors.update();
    
    int sensors_on_line = 0;
    for(int i = 0; i < 4; i++) { // Revisar sensores principales
        if(sensors.isOnLine(i)) {
            sensors_on_line++;
        }
    }
    
    // Si 3 o más sensores detectan línea = intersección
    return sensors_on_line >= 3;
}

inline bool LineFollower::detectLineEnd() {
    sensors.update();
    
    // Fin de línea: ningún sensor detecta línea
    for(int i = 0; i < 4; i++) {
        if(sensors.isOnLine(i)) return false;
    }
    
    return true;
}

inline void LineFollower::updateState() {
    if(sensors.detectAnyEdge()) {
        current_state = LineFollowState::EDGE_DETECTED;
    }
    else if(detectIntersection()) {
        current_state = LineFollowState::INTERSECTION_DETECTED;
    }
    else if(sensors.isLineDetected()) {
        current_state = LineFollowState::ON_LINE;
    }
    else {
        current_state = LineFollowState::LINE_LOST;
    }
}

inline float LineFollower::calculatePID(float error) {
    unsigned long current_time = millis();
    float dt = (current_time - last_time) / 1000.0;
    
    if(dt <= 0) dt = 0.01;
    
    // Término proporcional
    float p_term = kp * error;
    
    // Término integral con anti-windup
    integral += error * dt;
    integral = constrain(integral, -PID::LineFollower::MAX_CORRECTION/ki, 
                                    PID::LineFollower::MAX_CORRECTION/ki);
    float i_term = ki * integral;
    
    // Término derivativo
    float derivative = (error - previous_error) / dt;
    float d_term = kd * derivative;
    
    // Salida total
    float output = p_term + i_term + d_term;
    output = constrain(output, -PID::LineFollower::MAX_CORRECTION, 
                                PID::LineFollower::MAX_CORRECTION);
    
    // Guardar para siguiente iteración
    previous_error = error;
    last_time = current_time;
    
    return output;
}

inline void LineFollower::applyCorrection(float correction, uint8_t base_speed) {
    // Convertir corrección a diferencia de velocidad en ruedas
    // correction: -100 (muy a la izquierda) a +100 (muy a la derecha)
    
    // Calcular velocidades de cada lado
    float left_speed = base_speed + correction;
    float right_speed = base_speed - correction;
    
    // Limitar velocidades
    left_speed = constrain(left_speed, -Speed::MAX_SPEED, Speed::MAX_SPEED);
    right_speed = constrain(right_speed, -Speed::MAX_SPEED, Speed::MAX_SPEED);
    
    // Para movimiento omnidireccional:
    // - Si correction > 0: robot debe girar a la derecha
    // - Si correction < 0: robot debe girar a la izquierda
    
    motors.moveOmni(base_speed, 0, correction * 0.5); // Factor de escala para rotación
}

inline void LineFollower::stop() {
    motors.stopAllMotors(true);
    previous_error = 0;
    integral = 0;
}

inline void LineFollower::setPID(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
    previous_error = 0;
    integral = 0;
    Serial.printf("Line follower PID updated: P=%.2f I=%.2f D=%.2f\n", kp, ki, kd);
}

inline LineFollowState LineFollower::getState() const {
    return current_state;
}

inline int16_t LineFollower::getLineError() const {
    return line_error;
}

inline bool LineFollower::isOnLine() const {
    return current_state == LineFollowState::ON_LINE;
}

#endif // LINE_FOLLOWER_H