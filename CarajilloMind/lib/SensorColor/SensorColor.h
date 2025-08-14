#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

class SensorColor {
public:
    SensorColor();
    bool begin();  // Inicializa el sensor
    void leerColor();  // Lee y muestra el color por Serial
    uint16_t getR();
    uint16_t getG();
    uint16_t getB();
    uint16_t getC();

private:
    Adafruit_TCS34725 tcs;
    uint16_t r, g, b, c;
};

#endif
