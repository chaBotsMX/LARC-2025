#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// ============================================
// MOTORES DC - DRV8256E
// ============================================
namespace MotorPins {
    // Motor Frontal Izquierdo
    constexpr uint8_t FL_IN1 = 2;
    constexpr uint8_t FL_IN2 = 3;
    constexpr uint8_t FL_PWM = 4;
    
    // Motor Frontal Derecho
    constexpr uint8_t FR_IN1 = 5;
    constexpr uint8_t FR_IN2 = 6;
    constexpr uint8_t FR_PWM = 7;
    
    // Motor Trasero Izquierdo
    constexpr uint8_t BL_IN1 = 8;
    constexpr uint8_t BL_IN2 = 9;
    constexpr uint8_t BL_PWM = 10;
    
    // Motor Trasero Derecho
    constexpr uint8_t BR_IN1 = 11;
    constexpr uint8_t BR_IN2 = 12;
    constexpr uint8_t BR_PWM = 24;
}

// ============================================
// MOTOR STEPPER - DRV8834 (Carrusel)
// ============================================
namespace StepperPins {
    constexpr uint8_t STEP = 25;
    constexpr uint8_t DIR = 26;
    constexpr uint8_t ENABLE = 27;
}

// ============================================
// SENSORES DE LÍNEA - TCRT5000
// ============================================
namespace LineSensorPins {
    // Frontales (diagonal)
    constexpr uint8_t FRONT_LEFT = A0;
    constexpr uint8_t FRONT_RIGHT = A1;
    
    // Traseros (diagonal)
    constexpr uint8_t BACK_LEFT = A2;
    constexpr uint8_t BACK_RIGHT = A3;
    
    // Laterales adicionales
    constexpr uint8_t LEFT_FRONT = A4;
    constexpr uint8_t LEFT_BACK = A5;
    constexpr uint8_t RIGHT_FRONT = A6;
    constexpr uint8_t RIGHT_BACK = A7;
}

// ============================================
// I2C - Buses y direcciones
// ============================================
namespace I2C {
    // Pines I2C principales (Wire)
    constexpr uint8_t SDA_PIN = 18;
    constexpr uint8_t SCL_PIN = 19;
    
    // Pines I2C secundarios (Wire1) si se necesita
    constexpr uint8_t SDA1_PIN = 17;
    constexpr uint8_t SCL1_PIN = 16;
    
    // Direcciones de dispositivos
    namespace Address {
        constexpr uint8_t BNO055 = 0x28;
        constexpr uint8_t TCA9548A = 0x70;  // Multiplexor
        constexpr uint8_t PCA9685 = 0x40;   // Driver PWM servos
        constexpr uint8_t VL53L0X_DEFAULT = 0x29;
        constexpr uint8_t TCS34725_DEFAULT = 0x29;
    }
    
    // Canales del multiplexor TCA9548A
    namespace MuxChannel {
        constexpr uint8_t TOF_FRONT_LEFT = 0;
        constexpr uint8_t TOF_FRONT_RIGHT = 1;
        constexpr uint8_t TOF_BACK_LEFT = 2;
        constexpr uint8_t TOF_BACK_RIGHT = 3;
        constexpr uint8_t TOF_ARM = 4;
        constexpr uint8_t COLOR_SENSOR_1 = 5;
        constexpr uint8_t COLOR_SENSOR_2 = 6;
        constexpr uint8_t COLOR_SENSOR_3 = 7;
    }
}

// ============================================
// SERVOS - Controlados por PCA9685
// ============================================
namespace ServoChannels {
    // Brazo de recolección
    constexpr uint8_t ARM_LIFT = 0;      // MG995 - Levanta el brazo
    constexpr uint8_t ARM_EXTEND = 1;    // SG90 - Extiende el brazo
    constexpr uint8_t ARM_GRAB = 2;      // SG90 - Pinza de agarre
    
    // Sistema de clasificación
    constexpr uint8_t CAROUSEL_GATE = 3; // SG90 - Compuerta del carrusel
    constexpr uint8_t ELEVATOR_GATE = 4; // SG90 - Compuerta del elevador
}

// ============================================
// CONTROLES Y BOTONES
// ============================================
namespace Controls {
    constexpr uint8_t START_BUTTON = 28;
    constexpr uint8_t EMERGENCY_STOP = 29;
    constexpr uint8_t MODE_SWITCH = 30;  // AUTO/MANUAL
}

// ============================================
// LEDS INDICADORES
// ============================================
namespace LEDs {
    constexpr uint8_t STATUS_LED = 13;    // LED onboard
    constexpr uint8_t READY_LED = 31;
    constexpr uint8_t ERROR_LED = 32;
}

// ============================================
// COMUNICACIÓN RF (Modo manual)
// ============================================
namespace RFPins {
    constexpr uint8_t RX_PIN = 0;
    constexpr uint8_t TX_PIN = 1;
}

#endif // PINS_H