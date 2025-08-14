#include "SensorColor.h"

//constructor
SensorColor::SensorColor() {
    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
}

bool SensorColor::begin() {
    
    if (tcs.begin()) {
        Serial.println("TCS34725 encontrado!");
        return true;
    } else {
        Serial.println("No se detecta el TCS34725.");
        return false;
    }
}

void SensorColor::leerColor() {
    tcs.getRawData(&r, &g, &b, &c);
}

uint16_t SensorColor::getR() { return r; }
uint16_t SensorColor::getG() { return g; }
uint16_t SensorColor::getB() { return b; }
uint16_t SensorColor::getC() { return c; }
