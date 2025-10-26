#ifndef SERVOS_H
#define SERVOS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "../Config/pins.h"
#include "../Config/constants.h"

// ============================================
// CLASE DE CONTROL DE SERVOS
// ============================================

class ServoController {
public:
    ServoController();
    
    // Inicialización
    bool begin(TwoWire& wire = Wire);
    
    // Control directo (microsegundos)
    void setServo(uint8_t channel, uint16_t pulse_us);
    
    // Control por ángulo (0-180°)
    void setServoAngle(uint8_t channel, uint8_t angle);
    
    // Movimiento suave
    void moveServoSmooth(uint8_t channel, uint16_t target_us, uint16_t duration_ms);
    
    // Funciones específicas del brazo
    void armRetract();
    void armExtendToLevel(uint8_t level);  // 1, 2, o 3
    void gripperOpen();
    void gripperClose();
    bool isGripperClosed() const;
    
    // Funciones del sistema de clasificación
    void carouselClose();
    void carouselToRipe();
    void carouselToOverripe();
    void elevatorOpen();
    void elevatorClose();
    
    // Posición inicial segura
    void moveToSafePosition();
    
    // Estado
    uint16_t getServoPosition(uint8_t channel) const;
    bool isMoving() const;
    
    // Debug
    void printServoPositions();
    
private:
    Adafruit_PWMServoDriver pwm;
    uint16_t current_positions[16];  // Posiciones actuales en microsegundos
    bool initialized;
    
    // Constantes de conversión para PCA9685
    static constexpr float PWM_FREQUENCY = 50.0;  // 50Hz para servos
    static constexpr float CLOCK_FREQUENCY = 25000000.0;  // 25MHz
    static constexpr float PULSE_LENGTH = 1000000.0 / PWM_FREQUENCY / 4096.0;
    
    // Conversión
    uint16_t microsecondsToPWM(uint16_t microseconds);
    uint16_t angleToPulse(uint8_t angle);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline ServoController::ServoController() 
    : pwm(I2C::Address::PCA9685),
      initialized(false)
{
    for(int i = 0; i < 16; i++) {
        current_positions[i] = 1500; // Posición neutral
    }
}

inline bool ServoController::begin(TwoWire& wire) {
    Serial.println("Initializing PCA9685 servo driver...");
    
    pwm.begin();
    pwm.setPWMFreq(PWM_FREQUENCY);
    
    delay(10);
    
    initialized = true;
    
    // Mover todos los servos a posición segura
    moveToSafePosition();
    
    Serial.println("Servo controller initialized");
    return true;
}

inline void ServoController::setServo(uint8_t channel, uint16_t pulse_us) {
    if(!initialized || channel >= 16) return;
    
    // Limitar rango seguro (500us - 2500us)
    pulse_us = constrain(pulse_us, 500, 2500);
    
    uint16_t pwm_value = microsecondsToPWM(pulse_us);
    pwm.setPWM(channel, 0, pwm_value);
    
    current_positions[channel] = pulse_us;
}

inline void ServoController::setServoAngle(uint8_t channel, uint8_t angle) {
    uint16_t pulse = angleToPulse(angle);
    setServo(channel, pulse);
}

inline void ServoController::moveServoSmooth(uint8_t channel, uint16_t target_us, uint16_t duration_ms) {
    if(!initialized || channel >= 16) return;
    
    uint16_t start_pos = current_positions[channel];
    uint16_t steps = duration_ms / 20;  // 20ms por paso (~50Hz)
    
    if(steps == 0) steps = 1;
    
    int16_t delta = target_us - start_pos;
    float step_size = (float)delta / steps;
    
    for(uint16_t i = 0; i < steps; i++) {
        uint16_t pos = start_pos + (step_size * i);
        setServo(channel, pos);
        delay(20);
    }
    
    // Asegurar posición final exacta
    setServo(channel, target_us);
}

inline uint16_t ServoController::microsecondsToPWM(uint16_t microseconds) {
    // Convertir microsegundos a valor PWM para PCA9685
    return (uint16_t)(microseconds / PULSE_LENGTH);
}

inline uint16_t ServoController::angleToPulse(uint8_t angle) {
    // Convertir ángulo (0-180°) a pulso (1000-2000us)
    // Mapeo: 0° = 1000us, 90° = 1500us, 180° = 2000us
    angle = constrain(angle, 0, 180);
    return map(angle, 0, 180, 1000, 2000);
}

// ============================================
// FUNCIONES ESPECÍFICAS DEL BRAZO
// ============================================

inline void ServoController::armRetract() {
    Serial.println("Retracting arm...");
    
    // Primero cerrar pinza
    gripperClose();
    delay(300);
    
    // Retraer extensión
    moveServoSmooth(ServoChannels::ARM_EXTEND, 
                    ServoPos::ArmExtend::RETRACTED, 
                    500);
    
    // Bajar brazo
    moveServoSmooth(ServoChannels::ARM_LIFT, 
                    ServoPos::ArmLift::RETRACTED, 
                    800);
}

inline void ServoController::armExtendToLevel(uint8_t level) {
    if(level < 1 || level > 3) {
        Serial.println("ERROR: Invalid arm level");
        return;
    }
    
    Serial.printf("Extending arm to level %d...\n", level);
    
    uint16_t lift_position;
    
    switch(level) {
        case 1:
            lift_position = ServoPos::ArmLift::LEVEL_1;
            break;
        case 2:
            lift_position = ServoPos::ArmLift::LEVEL_2;
            break;
        case 3:
            lift_position = ServoPos::ArmLift::LEVEL_3;
            break;
        default:
            lift_position = ServoPos::ArmLift::RETRACTED;
    }
    
    // Abrir pinza primero
    gripperOpen();
    delay(200);
    
    // Levantar brazo a la altura
    moveServoSmooth(ServoChannels::ARM_LIFT, lift_position, 800);
    
    // Extender brazo
    moveServoSmooth(ServoChannels::ARM_EXTEND, 
                    ServoPos::ArmExtend::EXTENDED, 
                    500);
    
    delay(300); // Estabilizar
}

inline void ServoController::gripperOpen() {
    setServo(ServoChannels::ARM_GRAB, ServoPos::Gripper::OPEN);
}

inline void ServoController::gripperClose() {
    setServo(ServoChannels::ARM_GRAB, ServoPos::Gripper::CLOSED);
}

inline bool ServoController::isGripperClosed() const {
    uint16_t pos = current_positions[ServoChannels::ARM_GRAB];
    return abs(pos - ServoPos::Gripper::CLOSED) < 50;
}

// ============================================
// FUNCIONES DEL SISTEMA DE CLASIFICACIÓN
// ============================================

inline void ServoController::carouselClose() {
    setServo(ServoChannels::CAROUSEL_GATE, ServoPos::CarouselGate::CLOSED);
}

inline void ServoController::carouselToRipe() {
    Serial.println("Carousel -> RIPE container (red)");
    setServo(ServoChannels::CAROUSEL_GATE, ServoPos::CarouselGate::RIPE);
}

inline void ServoController::carouselToOverripe() {
    Serial.println("Carousel -> OVERRIPE container (blue)");
    setServo(ServoChannels::CAROUSEL_GATE, ServoPos::CarouselGate::OVERRIPE);
}

inline void ServoController::elevatorOpen() {
    setServo(ServoChannels::ELEVATOR_GATE, ServoPos::ElevatorGate::OPEN);
}

inline void ServoController::elevatorClose() {
    setServo(ServoChannels::ELEVATOR_GATE, ServoPos::ElevatorGate::CLOSED);
}

// ============================================
// UTILIDADES
// ============================================

inline void ServoController::moveToSafePosition() {
    Serial.println("Moving servos to safe position...");
    
    // Posiciones seguras para todos los servos
    setServo(ServoChannels::ARM_LIFT, ServoPos::ArmLift::RETRACTED);
    delay(100);
    setServo(ServoChannels::ARM_EXTEND, ServoPos::ArmExtend::RETRACTED);
    delay(100);
    setServo(ServoChannels::ARM_GRAB, ServoPos::Gripper::OPEN);
    delay(100);
    setServo(ServoChannels::CAROUSEL_GATE, ServoPos::CarouselGate::CLOSED);
    delay(100);
    setServo(ServoChannels::ELEVATOR_GATE, ServoPos::ElevatorGate::CLOSED);
    
    delay(500); // Dar tiempo a que se muevan
}

inline uint16_t ServoController::getServoPosition(uint8_t channel) const {
    if(channel >= 16) return 0;
    return current_positions[channel];
}

inline bool ServoController::isMoving() const {
    // Esta es una simplificación - en realidad no sabemos si el servo
    // terminó de moverse, solo si le enviamos un comando reciente
    return false;
}

inline void ServoController::printServoPositions() {
    Serial.println("\n=== Servo Positions ===");
    Serial.printf("Arm Lift:    %d us\n", current_positions[ServoChannels::ARM_LIFT]);
    Serial.printf("Arm Extend:  %d us\n", current_positions[ServoChannels::ARM_EXTEND]);
    Serial.printf("Gripper:     %d us %s\n", 
        current_positions[ServoChannels::ARM_GRAB],
        isGripperClosed() ? "(CLOSED)" : "(OPEN)"
    );
    Serial.printf("Carousel:    %d us\n", current_positions[ServoChannels::CAROUSEL_GATE]);
    Serial.printf("Elevator:    %d us\n", current_positions[ServoChannels::ELEVATOR_GATE]);
    Serial.println("=====================\n");
}

#endif // SERVOS_H