#include "pid.h"

PID::PID() : PID(1.0, 0.0, 0.0, -100.0, 100.0) {
    // Constructor por defecto con ganancias Kp=1, Ki=0, Kd=0 y límites de salida -100 a 100
}
PID::PID(float kp, float ki, float kd, float outputMin, float outputMax) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->outputMin = outputMin;
    this->outputMax = outputMax;
    
    setpoint = 0;
    lastError = 0;
    integral = 0;
    lastTime = 0;
    
    pTerm = 0;
    iTerm = 0;
    dTerm = 0;
    error = 0;
}

void PID::setGains(float kp, float ki, float kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

void PID::setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
}

void PID::setSetpoint(float setpoint) {
    this->setpoint = setpoint;
}

void PID::reset() {
    lastError = 0;
    integral = 0;
    lastTime = millis();
    pTerm = 0;
    iTerm = 0;
    dTerm = 0;
    error = 0;
}

float PID::compute(float input) {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0; // Convertir a segundos
    
    if (lastTime == 0 || dt <= 0 || dt > 1.0) {
        // Primera ejecución o dt inválido
        lastTime = now;
        lastError = setpoint - input;
        return 0;
    }
    
    lastTime = now;
    return compute(input, dt);
}

float PID::compute(float input, float dt) {
    // Calcular error
    error = setpoint - input;
    
    // Término proporcional
    pTerm = kp * error;
    
    // Término integral (con anti-windup)
    integral += error * dt;
    
    // Anti-windup: limitar integral
    float maxIntegral = outputMax / (ki + 0.001); // Evitar división por cero
    integral = constrain(integral, -maxIntegral, maxIntegral);
    
    iTerm = ki * integral;
    
    // Término derivativo
    float derivative = (error - lastError) / dt;
    dTerm = kd * derivative;
    
    // Calcular salida
    float output = pTerm + iTerm + dTerm;
    
    // Limitar salida
    output = constrain(output, outputMin, outputMax);
    
    // Guardar error para próxima iteración
    lastError = error;
    
    return output;
}