#include "SensorColor.h"


SensorColor::SensorColor()
: tcs(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_4X), r(0), g(0), b(0), c(0) {}
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

void SensorColor::printColor() {
    Serial.print(" R: "); Serial.print(getR());
    Serial.print(" G: "); Serial.print(getG());
    Serial.print(" B: "); Serial.print(getB());
    Serial.print(" C: "); Serial.println(getC());
}



