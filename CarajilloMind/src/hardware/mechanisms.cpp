#include "mechanisms.h"

Mechanisms mechanisms;

void Mechanisms::init() {
    pwm = Adafruit_PWMServoDriver(PWM_DRIVER_ADDR);
    pwm.begin();
    pwm.setPWMFreq(50); // 50 Hz para servos
    
    // Configurar pines del stepper
    pinMode(STEPPER_STEP, OUTPUT);
    pinMode(STEPPER_DIR, OUTPUT);
    pinMode(STEPPER_ENABLE, OUTPUT);
    
    // Posiciones iniciales
    retractArm();
    lowerElevator();
    rotateCarouselTo(CAROUSEL_CENTER);
    openGripper();
    disableStepper();
}

void Mechanisms::setServo(uint8_t num, int angle) {
    angle = constrain(angle, 0, 180);
    int pulseLen = map(angle, 0, 180, 150, 600); // Ajustar según servos
    pwm.setPWM(num, 0, pulseLen);
}

void Mechanisms::setArmPosition(int angle) {
    setServo(SERVO_ARM, angle);
}

void Mechanisms::extendArm() {
    setArmPosition(ARM_HIGH_LEVEL);
    delay(500);
}

void Mechanisms::retractArm() {
    setArmPosition(ARM_RETRACTED);
    delay(500);
}

void Mechanisms::positionArmForLevel(TreeLevel level) {
    switch (level) {
        case LOW:
            setArmPosition(ARM_LOW_LEVEL);
            break;
        case MIDDLE:
            setArmPosition(ARM_MID_LEVEL);
            break;
        case HIGH:
            setArmPosition(ARM_HIGH_LEVEL);
            break;
    }
    delay(800);
}

void Mechanisms::raiseElevator() {
    setServo(SERVO_ELEVATOR_1, ELEVATOR_UP);
    setServo(SERVO_ELEVATOR_2, ELEVATOR_UP);
    delay(1000);
}

void Mechanisms::lowerElevator() {
    setServo(SERVO_ELEVATOR_1, ELEVATOR_DOWN);
    setServo(SERVO_ELEVATOR_2, ELEVATOR_DOWN);
    delay(1000);
}

void Mechanisms::rotateCarouselTo(int angle) {
    setServo(SERVO_CAROUSEL, angle);
    delay(500);
}

void Mechanisms::positionCarouselForBean(BeanType type) {
    switch (type) {
        case RIPE:
            rotateCarouselTo(CAROUSEL_RIPE);
            break;
        case OVERRIPE:
            rotateCarouselTo(CAROUSEL_OVERRIPE);
            break;
        default:
            rotateCarouselTo(CAROUSEL_CENTER);
            break;
    }
}

void Mechanisms::openGripper() {
    setServo(SERVO_GRIPPER, GRIPPER_OPEN);
    delay(300);
}

void Mechanisms::closeGripper() {
    setServo(SERVO_GRIPPER, GRIPPER_CLOSED);
    delay(300);
}

void Mechanisms::enableStepper() {
    digitalWrite(STEPPER_ENABLE, LOW);
}

void Mechanisms::disableStepper() {
    digitalWrite(STEPPER_ENABLE, HIGH);
}

void Mechanisms::moveStepper(int steps, bool direction) {
    enableStepper();
    digitalWrite(STEPPER_DIR, direction ? HIGH : LOW);
    
    for (int i = 0; i < steps; i++) {
        digitalWrite(STEPPER_STEP, HIGH);
        delayMicroseconds(500);
        digitalWrite(STEPPER_STEP, LOW);
        delayMicroseconds(500);
    }
    
    disableStepper();
}