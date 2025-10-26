#include "SensorColor.h"
#include "ColorConverterLib.h"

// Define el objeto global del sensor
SensorColor sensorC; 
  
SensorColor::SensorColor()
// Inicialización de variables
: tcs(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X), r_raw(0), g_raw(0), b_raw(0), c_raw(0), 
  r_8bit(0), g_8bit(0), b_8bit(0), hue(0.0), saturation(0.0), value(0.0) {}

bool SensorColor::begin() {
    
    if (tcs.begin()) {
        Serial.println("TCS34725 encontrado!");
        // Opcional: Encender el LED
        // tcs.setInterrupt(false); 
        return true;
    } else {
        Serial.println("No se detecta el TCS34725.");
        return false;
    }
}

// Implementación de la función de calibración
void SensorColor::calibrarBlanco() {
    // Lectura para establecer el punto blanco de referencia
    tcs.getRawData(&r_raw, &g_raw, &b_raw, &c_raw);
    
    // Se usa el valor C_raw como el valor máximo de referencia (c_blanco)
    c_blanco = c_raw; 
    Serial.print("Calibración de Blanco (C_blanco) establecida en: ");
    Serial.println(c_blanco);
}


void SensorColor::leerColor() {
    // 1. Lectura de valores RAW (16 bits) del sensor
    tcs.getRawData(&r_raw, &g_raw, &b_raw, &c_raw);
    
    // 2. Conversión normalizada a 8 bits (0-255)
    rawToNormalizado();
    
    // 3. Conversión de RGB (8 bits) a HSV
    rgbToHsv();
}

void SensorColor::rawToNormalizado() {
    
    // Evita la división por cero y asegura que la calibración se haya realizado
    if (c_blanco == 0 || c_raw == 0) { 
        r_8bit = g_8bit = b_8bit = 0;
        return;
    }

    // FÓRMULA DE NORMALIZACIÓN:
    // 1. Divide cada canal por el valor Clear (C) para obtener una proporción de color.
    // 2. Multiplica por 255 (o 256) para obtener el rango de 8 bits.
    
    // Normalizar a una escala de 0 a 1 basada en C_raw
    float ratio_r = (float)r_raw / c_raw;
    float ratio_g = (float)g_raw / c_raw;
    float ratio_b = (float)b_raw / c_raw;

    // Escalar al rango 0-255 y asegurar que no excedan 255
    r_8bit = constrain((int)(ratio_r * 255.0), 0, 255);
    g_8bit = constrain((int)(ratio_g * 255.0), 0, 255);
    b_8bit = constrain((int)(ratio_b * 255.0), 0, 255);
}

void SensorColor::rgbToHsv() {
    ColorConverter::RgbToHsv(r_8bit, g_8bit, b_8bit, hue, saturation, value);
}

void SensorColor::imprimir() {
    Serial.print("-----------------------------\n");
    Serial.print(r_8bit); Serial.print("r: ");
    Serial.print(g_8bit); Serial.print("g: ");
    Serial.print(b_8bit); Serial.print("b: ");

    // Imprimimos H multiplicado por 360 para mostrar el tono en grados (0° - 360°)
    Serial.print("HSV: H="); 
    Serial.print(hue * 360.0, 1); 
    Serial.print(" S="); 
    Serial.print(saturation, 2); 
    Serial.print(" V="); 
    Serial.println(value, 2);
}

void SensorColor::definir() {
    // Multiplicamos 'hue' por 360 para trabajar en grados (0-360)
    double hue_grados = hue * 360.0;

    // Condición especial para el ROJO, ya que se encuentra al inicio y al final del círculo cromático
    if (hue_grados >= 345 || hue_grados <= 15) {
        // También validamos que tenga suficiente saturación y brillo para no confundirlo con blanco o negro
        if (saturation > 0.4 && value > 0.3) {
            Serial.println("Color: Rojo");
        }
    } 
    else if (hue_grados > 25.0 && hue_grados <= 35.0) {
        Serial.println("Color: Amarillo");
    } 
    else if (hue_grados > 35.0 && hue_grados <= 90.0) {
        Serial.println("Color: Verde");
    } 
    else if (hue_grados > 150.0 && hue_grados <= 260.0) {
        Serial.println("Color: Azul");
    } 
    else {

    }

}