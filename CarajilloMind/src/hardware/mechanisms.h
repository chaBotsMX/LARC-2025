#ifndef MECHANISMS_H
#define MECHANISMS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "../config.h"

class Mechanisms {
public:
    void init();
    
    // Brazo
    void setArmPosition(int angle);
    void raiseArm();
    void retractArm();
    
    // Pisos
    void grabFirstLevel();
    void grabSecondLevel();
    
    // Carousel
    void rotateCarouselTo(int angle);
    void positionCarouselForBean(BeanType type);
    
    // Gripper
    void openGripper();
    void closeGripper();
    
    // Stepper
    void moveStepper(int steps, bool direction);
    void enableStepper();
    void disableStepper();
    
private:
    Adafruit_PWMServoDriver pwm;
    void setServo(uint8_t num, int angle);
};

extern Mechanisms mechanisms;

#endif