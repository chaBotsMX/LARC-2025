#include "Adafruit_VL53L0X.h"
#include "SensorDistancia.h"
// No es necesario incluir SensorColor.h a menos que se use aquí

// GLOBAL 
SensorDistancia sensorD; 
bool openField = false;
// ELIMINAR ESTA LÍNEA (EVITA EL ERROR DE DOBLE DEFINICIÓN):
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Constructor de la clase
SensorDistancia::SensorDistancia() : lox() {
    // Inicialización de lox como miembro de la clase
}

bool SensorDistancia::begin() {
    Serial.println("Inicializando VL53L0X...");
    
    // lox aquí es el miembro privado de SensorDistancia
    if (!lox.begin()) {
        Serial.println(F("Error: No se pudo iniciar el VL53L0X"));
        return false;
    }
    
    // Iniciar el modo de rango continuo
    lox.startRangeContinuous();
    Serial.println(F("VL53L0X iniciado."));
    return true;
}


void SensorDistancia::distancia() {
    if (lox.isRangeComplete()) {
        uint16_t rango_mm = lox.readRange(); // La lectura está en mm
        
        Serial.print("Distancia en cm: ");
        // CORRECCIÓN: mm / 10 = cm
        Serial.println((float)rango_mm / 10.0);
    }
}

float SensorDistancia::getDistanciaMm() {
    if (lox.isRangeComplete()) {
        uint16_t rango_mm = lox.readRange(); // La lectura está en mm
        return (float)rango_mm; // Devolver en mm
    }
    return -1.0; // Valor de error si no hay lectura disponible
}

void SensorDistancia::pasar(uint16_t rango_mm) {
    if ((rango_mm/10.0 ) > 300) {
        openField = true;
    } else {
        openField = false;  
    }
} //cuando openField el robot pasa por las albercas 

