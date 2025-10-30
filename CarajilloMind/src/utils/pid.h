#ifndef PID_H
#define PID_H

#include <Arduino.h>

class PID {
public:
    PID();
    PID(float kp, float ki, float kd, float outputMin, float outputMax);
    
    void setGains(float kp, float ki, float kd);
    void setOutputLimits(float min, float max);
    void setSetpoint(float setpoint);
    void reset();
    
    float compute(float input);
    float compute(float input, float dt); // Con delta time manual
    
    // Getters para debugging
    float getP() { return pTerm; }
    float getI() { return iTerm; }
    float getD() { return dTerm; }
    float getError() { return error; }
    
private:
    // Ganancias
    float kp;
    float ki;
    float kd;
    
    // Límites de salida
    float outputMin;
    float outputMax;
    
    // Variables de estado
    float setpoint;
    float lastError;
    float integral;
    unsigned long lastTime;
    
    // Para debugging
    float pTerm;
    float iTerm;
    float dTerm;
    float error;
};

#endif