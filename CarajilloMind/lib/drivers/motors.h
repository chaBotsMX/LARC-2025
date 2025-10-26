#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include "../Config/pins.h"
#include "../Config/constants.h"

// ============================================
// ENUMERACIONES
// ============================================

enum class MotorID {
    FRONT_LEFT = 0,
    FRONT_RIGHT = 1,
    BACK_LEFT = 2,
    BACK_RIGHT = 3
};

enum class Direction {
    FORWARD,
    BACKWARD,
    BRAKE,
    COAST
};

// ============================================
// ESTRUCTURA DE MOTOR INDIVIDUAL
// ============================================

struct Motor {
    uint8_t pin_in1;
    uint8_t pin_in2;
    uint8_t pin_pwm;
    int16_t current_speed;      // -255 a +255
    float speed_correction;     // Factor de corrección
    bool is_reversed;
    
    Motor(uint8_t in1, uint8_t in2, uint8_t pwm) 
        : pin_in1(in1), pin_in2(in2), pin_pwm(pwm),
          current_speed(0), speed_correction(1.0), is_reversed(false) {}
};

// ============================================
// CLASE DE CONTROL DE MOTORES
// ============================================

class MotorController {
public:
    MotorController();
    
    // Inicialización
    void begin();
    
    // Control individual de motores
    void setMotorSpeed(MotorID motor, int16_t speed);  // -255 a +255
    void setMotorDirection(MotorID motor, Direction dir);
    void stopMotor(MotorID motor, bool brake = true);
    
    // Control de todos los motores
    void setAllMotors(int16_t speed);
    void stopAllMotors(bool brake = true);
    void emergencyStop();
    
    // Movimiento omnidireccional
    void moveOmni(float vx, float vy, float omega);
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void strafeLeft(uint8_t speed);
    void strafeRight(uint8_t speed);
    void rotateClockwise(uint8_t speed);
    void rotateCounterClockwise(uint8_t speed);
    
    // Configuración
    void setSpeedCorrection(MotorID motor, float correction);
    void setReversed(MotorID motor, bool reversed);
    void loadCalibration(const CalibrationData& cal);
    
    // Estado
    int16_t getMotorSpeed(MotorID motor) const;
    bool isMoving() const;
    
    // Debug
    void printMotorStates();
    
private:
    Motor motors[4];
    
    // Helpers
    void applyMotorCommand(MotorID motor, int16_t speed);
    int16_t constrainSpeed(int16_t speed);
    uint8_t speedToAbsolute(int16_t speed);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline MotorController::MotorController() 
    : motors{
        Motor(MotorPins::FL_IN1, MotorPins::FL_IN2, MotorPins::FL_PWM),
        Motor(MotorPins::FR_IN1, MotorPins::FR_IN2, MotorPins::FR_PWM),
        Motor(MotorPins::BL_IN1, MotorPins::BL_IN2, MotorPins::BL_PWM),
        Motor(MotorPins::BR_IN1, MotorPins::BR_IN2, MotorPins::BR_PWM)
    } 
{}

inline void MotorController::begin() {
    // Configurar pines como salidas
    for(int i = 0; i < 4; i++) {
        pinMode(motors[i].pin_in1, OUTPUT);
        pinMode(motors[i].pin_in2, OUTPUT);
        pinMode(motors[i].pin_pwm, OUTPUT);
        
        // Inicializar en estado detenido
        digitalWrite(motors[i].pin_in1, LOW);
        digitalWrite(motors[i].pin_in2, LOW);
        analogWrite(motors[i].pin_pwm, 0);
    }
    
    // Configurar frecuencia PWM (Teensy 4.1 soporta hasta 150kHz)
    analogWriteFrequency(MotorPins::FL_PWM, 20000); // 20kHz
    analogWriteFrequency(MotorPins::FR_PWM, 20000);
    analogWriteFrequency(MotorPins::BL_PWM, 20000);
    analogWriteFrequency(MotorPins::BR_PWM, 20000);
    
    Serial.println("Motors initialized");
}

inline void MotorController::setMotorSpeed(MotorID motor, int16_t speed) {
    uint8_t idx = static_cast<uint8_t>(motor);
    
    // Aplicar corrección de velocidad
    speed = speed * motors[idx].speed_correction;
    
    // Aplicar inversión si es necesario
    if(motors[idx].is_reversed) {
        speed = -speed;
    }
    
    // Limitar rango
    speed = constrainSpeed(speed);
    
    // Guardar velocidad actual
    motors[idx].current_speed = speed;
    
    // Aplicar comando al hardware
    applyMotorCommand(motor, speed);
}

inline void MotorController::applyMotorCommand(MotorID motor, int16_t speed) {
    uint8_t idx = static_cast<uint8_t>(motor);
    Motor& m = motors[idx];
    
    if(speed > 0) {
        // Forward
        digitalWrite(m.pin_in1, HIGH);
        digitalWrite(m.pin_in2, LOW);
        analogWrite(m.pin_pwm, speedToAbsolute(speed));
    }
    else if(speed < 0) {
        // Backward
        digitalWrite(m.pin_in1, LOW);
        digitalWrite(m.pin_in2, HIGH);
        analogWrite(m.pin_pwm, speedToAbsolute(speed));
    }
    else {
        // Brake (corto circuito)
        digitalWrite(m.pin_in1, HIGH);
        digitalWrite(m.pin_in2, HIGH);
        analogWrite(m.pin_pwm, 255);
    }
}

inline void MotorController::stopMotor(MotorID motor, bool brake) {
    uint8_t idx = static_cast<uint8_t>(motor);
    Motor& m = motors[idx];
    
    motors[idx].current_speed = 0;
    
    if(brake) {
        // Brake - cortocircuito para frenar rápido
        digitalWrite(m.pin_in1, HIGH);
        digitalWrite(m.pin_in2, HIGH);
        analogWrite(m.pin_pwm, 255);
    } else {
        // Coast - dejar que el motor se detenga por inercia
        digitalWrite(m.pin_in1, LOW);
        digitalWrite(m.pin_in2, LOW);
        analogWrite(m.pin_pwm, 0);
    }
}

inline void MotorController::stopAllMotors(bool brake) {
    for(int i = 0; i < 4; i++) {
        stopMotor(static_cast<MotorID>(i), brake);
    }
}

inline void MotorController::emergencyStop() {
    stopAllMotors(true);
    Serial.println("EMERGENCY STOP ACTIVATED!");
}

// ============================================
// CINEMÁTICA OMNIDIRECCIONAL
// ============================================

inline void MotorController::moveOmni(float vx, float vy, float omega) {
    /*
     * Para ruedas omnidireccionales en configuración de cruz (X):
     * 
     *        FL(+)  FR(+)
     *           \ /
     *            X
     *           / \
     *        BL(+)  BR(+)
     * 
     * vx: velocidad en X (adelante/atrás)
     * vy: velocidad en Y (izquierda/derecha)  
     * omega: velocidad angular (rotación)
     */
    
    // Constante geométrica para ruedas en X
    const float SQRT2 = 1.414213562;
    
    // Calcular velocidad de cada rueda
    int16_t v_fl = (vx - vy - omega * Robot::WHEEL_BASE_MM) / SQRT2;
    int16_t v_fr = (vx + vy + omega * Robot::WHEEL_BASE_MM) / SQRT2;
    int16_t v_bl = (vx + vy - omega * Robot::WHEEL_BASE_MM) / SQRT2;
    int16_t v_br = (vx - vy + omega * Robot::WHEEL_BASE_MM) / SQRT2;
    
    // Normalizar si alguna velocidad excede el máximo
    int16_t max_speed = max(max(abs(v_fl), abs(v_fr)), max(abs(v_bl), abs(v_br)));
    if(max_speed > Speed::MAX_SPEED) {
        float scale = (float)Speed::MAX_SPEED / max_speed;
        v_fl *= scale;
        v_fr *= scale;
        v_bl *= scale;
        v_br *= scale;
    }
    
    // Aplicar velocidades
    setMotorSpeed(MotorID::FRONT_LEFT, v_fl);
    setMotorSpeed(MotorID::FRONT_RIGHT, v_fr);
    setMotorSpeed(MotorID::BACK_LEFT, v_bl);
    setMotorSpeed(MotorID::BACK_RIGHT, v_br);
}

inline void MotorController::moveForward(uint8_t speed) {
    moveOmni(speed, 0, 0);
}

inline void MotorController::moveBackward(uint8_t speed) {
    moveOmni(-speed, 0, 0);
}

inline void MotorController::strafeLeft(uint8_t speed) {
    moveOmni(0, -speed, 0);
}

inline void MotorController::strafeRight(uint8_t speed) {
    moveOmni(0, speed, 0);
}

inline void MotorController::rotateClockwise(uint8_t speed) {
    moveOmni(0, 0, speed);
}

inline void MotorController::rotateCounterClockwise(uint8_t speed) {
    moveOmni(0, 0, -speed);
}

// ============================================
// UTILIDADES
// ============================================

inline int16_t MotorController::constrainSpeed(int16_t speed) {
    return constrain(speed, -255, 255);
}

inline uint8_t MotorController::speedToAbsolute(int16_t speed) {
    return abs(speed);
}

inline void MotorController::setSpeedCorrection(MotorID motor, float correction) {
    uint8_t idx = static_cast<uint8_t>(motor);
    motors[idx].speed_correction = correction;
}

inline void MotorController::setReversed(MotorID motor, bool reversed) {
    uint8_t idx = static_cast<uint8_t>(motor);
    motors[idx].is_reversed = reversed;
}

inline void MotorController::loadCalibration(const CalibrationData& cal) {
    for(int i = 0; i < 4; i++) {
        motors[i].speed_correction = cal.motors.speed_correction[i];
        motors[i].is_reversed = cal.motors.reversed[i];
    }
    Serial.println("Motor calibration loaded");
}

inline int16_t MotorController::getMotorSpeed(MotorID motor) const {
    return motors[static_cast<uint8_t>(motor)].current_speed;
}

inline bool MotorController::isMoving() const {
    for(int i = 0; i < 4; i++) {
        if(motors[i].current_speed != 0) return true;
    }
    return false;
}

inline void MotorController::printMotorStates() {
    Serial.println("\n=== Motor States ===");
    const char* names[] = {"FL", "FR", "BL", "BR"};
    for(int i = 0; i < 4; i++) {
        Serial.printf("%s: Speed=%d, Correction=%.2f, Reversed=%s\n",
            names[i],
            motors[i].current_speed,
            motors[i].speed_correction,
            motors[i].is_reversed ? "Yes" : "No"
        );
    }
    Serial.println("===================\n");
}

#endif // MOTORS_H