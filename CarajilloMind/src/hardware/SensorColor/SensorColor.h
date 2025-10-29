#ifndef SENSOR_COLOR_H
#define SENSOR_COLOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

class SensorColor {
public:
    SensorColor();
    bool begin();   
    void leerColor();  
    void imprimir();
    void definir();
    // NUEVA FUNCIÓN: Para establecer el valor C máximo al apuntar a algo blanco.
    void calibrarBlanco(); 
   
private:
    Adafruit_TCS34725 tcs;
    
    uint16_t r_raw, g_raw, b_raw, c_raw; 
    
    // Valor de referencia C para la calibración
    uint16_t c_blanco = 0; 
    
    uint8_t r_8bit, g_8bit, b_8bit;
    
    double hue, saturation, value;
    
    // Función renombrada para reflejar que hace una normalización por C_raw
    void rawToNormalizado(); 
    void rgbToHsv();
};

#endif