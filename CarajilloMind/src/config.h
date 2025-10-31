#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================
// PINES DE MOTORES
// ============================================
// Motor Front Left
#define MOTOR_FL_IN1 0
#define MOTOR_FL_IN2 1

// Motor Front Right
#define MOTOR_FR_IN1 2
#define MOTOR_FR_IN2 3

// Motor Rear Left
#define MOTOR_RL_IN1 4
#define MOTOR_RL_IN2 5

// Motor Rear Right
#define MOTOR_RR_IN1 6
#define MOTOR_RR_IN2 7

// Stepper Motor (DRV8834)
#define STEPPER_STEP 15
#define STEPPER_DIR 16
#define STEPPER_ENABLE 17   

// ============================================
// PINES DE SENSORES DE LÍNEA (TCRT5000)
// ============================================
#define LINE_FRONT_LEFT 22
#define LINE_FRONT_RIGHT 23
#define LINE_REAR_LEFT 24
#define LINE_REAR_RIGHT 25


// ============================================
// I2C BUSES
// ============================================
#define I2C_SDA 18
#define I2C_SCL 19

// Direcciones I2C
#define I2C_MULTIPLEXER_ADDR 0x70
#define PWM_DRIVER_ADDR 0x40
#define IMU_ADDR 0x28

// Canales del multiplexor para sensores
#define MUX_COLOR_1 0
#define MUX_COLOR_2 1
#define MUX_COLOR_3 2
#define MUX_COLOR_4 3
#define MUX_TOF_FL 4
#define MUX_TOF_FR 5
#define MUX_TOF_RL 6
#define MUX_TOF_RR 7

// ============================================
// PINES DE SERVOS (via PCA9685)
// ============================================
#define SERVO_ARM 0
#define SERVO_ELEVATOR_1 1
#define SERVO_ELEVATOR_2 2
#define SERVO_CAROUSEL 3
#define SERVO_GRIPPER 4

// ============================================
// BOTÓN DE INICIO
// ============================================
#define START_BUTTON 20

// ============================================
// CONSTANTES DE CALIBRACIÓN
// ============================================
// Sensores de línea
#define LINE_THRESHOLD 500
#define LINE_DETECTED true
#define LINE_NOT_DETECTED false

// Velocidades
#define BASE_SPEED 60
#define SLOW_SPEED 30
#define TURN_SPEED 100
#define ALIGN_SPEED 60

// Distancias (mm)
#define DISTANCE_POOL_MIN 100
#define DISTANCE_POOL_MAX 800
#define DISTANCE_OBSTACLE_STOP 150
#define DISTANCE_TREE_APPROACH 200

// Tiempos
#define EXIT_BOX_TIME 2000
#define ALIGN_TIMEOUT 5000
#define COLLECTION_TIMEOUT 30000

// ============================================
// POSICIONES DE SERVOS (0-180 grados)
// ============================================
// Brazo
#define ARM_RETRACTED 10
#define ARM_LOW_LEVEL 60
#define ARM_MID_LEVEL 90
#define ARM_HIGH_LEVEL 120

// Elevator
#define ELEVATOR_DOWN 0
#define ELEVATOR_UP 90

// Carousel
#define CAROUSEL_RIPE 45
#define CAROUSEL_OVERRIPE 135
#define CAROUSEL_CENTER 90

// Gripper
#define GRIPPER_OPEN 10
#define GRIPPER_CLOSED 90

// ============================================
// THRESHOLDS DE COLOR
// ============================================
// Ripe beans (red, orange, yellow)
#define RIPE_R_MIN 150
#define RIPE_R_MAX 255
#define RIPE_G_MIN 50
#define RIPE_G_MAX 200
#define RIPE_B_MIN 0
#define RIPE_B_MAX 100

// Green beans
#define GREEN_R_MIN 0
#define GREEN_R_MAX 100
#define GREEN_G_MIN 100
#define GREEN_G_MAX 255
#define GREEN_B_MIN 0
#define GREEN_B_MAX 100

// Overripe beans (blue, black)
#define OVERRIPE_B_MIN 100
#define OVERRIPE_B_MAX 255

// ============================================
// ESTADOS DEL ROBOT
// ============================================
enum RobotState {
    INIT,
    EXIT_BOX,
    NAVIGATE_TO_TREEZONE,
    COLLECT_LINE,
    RETURN_TO_BASE,
    DEPOSIT_BEANS,
    COMPLETE,
    ERROR
};

// ============================================
// TIPOS DE GRANOS
// ============================================
enum BeanType {
    NONE,
    RIPE,
    GREEN,
    OVERRIPE
};

// ============================================
// NIVELES DEL ÁRBOL
// ============================================
enum TreeLevel {
    LOWER,
    MIDDLE,
    UPPER
};

// ============================================
// DIRECCIONES DE MOVIMIENTO
// ============================================
enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

#endif