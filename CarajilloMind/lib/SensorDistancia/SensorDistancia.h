#ifndef SENSOR_DISTANCIA_H
#define SENSOR_DISTANCIA_H

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

class SensorDistancia {
public:
    SensorDistancia();
    bool begin(); // ¡Añadido! Necesitas inicializar el sensor D
    void distancia();
    void pasar(uint16_t rango_mm);

private:
    // Mantenemos el objeto aquí para una correcta encapsulación
    Adafruit_VL53L0X lox; 
};

#endif