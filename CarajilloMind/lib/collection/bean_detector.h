#ifndef BEAN_DETECTOR_H
#define BEAN_DETECTOR_H

#include <Arduino.h>
#include "../drivers/sensors.h"
#include "../config/constants.h"

// ============================================
// ESTRUCTURA DE DETECCIÓN DE GRANO
// ============================================

struct BeanDetection {
    bool is_present;
    BeanType type;
    uint16_t confidence;      // 0-100%
    uint16_t r, g, b;        // Valores RGB detectados
    uint8_t sensor_id;       // Cual sensor lo detectó
    
    BeanDetection() : is_present(false), type(BeanType::NONE), 
                     confidence(0), r(0), g(0), b(0), sensor_id(0) {}
};

// ============================================
// CLASE DETECTORA DE GRANOS
// ============================================

class BeanDetector {
public:
    BeanDetector(ColorSensors& color_sensors, DistanceSensors& distance_sensors);
    
    // Inicialización
    void begin();
    
    // Detección principal
    BeanDetection detectBean(uint8_t sensor_id = 0);
    bool isBeanPresent(uint8_t sensor_id = 0);
    BeanType identifyBeanType(uint8_t sensor_id = 0);
    
    // Clasificación por tipo de grano
    bool isRipe(BeanType type) const;
    bool isOverripe(BeanType type) const;
    bool isGreen(BeanType type) const;
    
    // Múltiples sensores
    BeanDetection scanAllSensors();
    int countBeansInRange();
    
    // Utilidades
    const char* beanTypeToString(BeanType type) const;
    void printDetection(const BeanDetection& detection);
    
private:
    ColorSensors& color_sensors;
    DistanceSensors& distance_sensors;
    
    // Último resultado
    BeanDetection last_detection;
    
    // Helpers
    BeanType classifyColorToBean(uint16_t r, uint16_t g, uint16_t b);
    uint16_t calculateConfidence(uint16_t r, uint16_t g, uint16_t b, BeanType type);
    bool isColorInRange(uint16_t r, uint16_t g, uint16_t b, 
                       const Threshold::Color::ColorRange& range);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline BeanDetector::BeanDetector(ColorSensors& color_sensors, 
                                   DistanceSensors& distance_sensors)
    : color_sensors(color_sensors),
      distance_sensors(distance_sensors)
{}

inline void BeanDetector::begin() {
    Serial.println("Bean detector initialized");
}

// ============================================
// DETECCIÓN PRINCIPAL
// ============================================

inline BeanDetection BeanDetector::detectBean(uint8_t sensor_id) {
    BeanDetection detection;
    detection.sensor_id = sensor_id;
    
    // Actualizar sensores
    color_sensors.update();
    distance_sensors.update();
    
    // Primero verificar si hay algo cerca (usando sensor de brazo)
    uint16_t distance = distance_sensors.getArm();
    
    if(distance > 100) { // Si está a más de 10cm, no hay grano
        detection.is_present = false;
        detection.type = BeanType::NONE;
        last_detection = detection;
        return detection;
    }
    
    // Leer color
    ColorReading color = color_sensors.getReading(sensor_id);
    
    detection.r = color.r;
    detection.g = color.g;
    detection.b = color.b;
    
    // Verificar que la lectura es válida (suficiente luz)
    if(color.c < Threshold::Color::MIN_BRIGHTNESS) {
        detection.is_present = false;
        detection.type = BeanType::UNKNOWN;
        detection.confidence = 0;
        last_detection = detection;
        return detection;
    }
    
    // Clasificar el color
    detection.type = classifyColorToBean(color.r, color.g, color.b);
    detection.confidence = calculateConfidence(color.r, color.g, color.b, detection.type);
    detection.is_present = (detection.type != BeanType::NONE && 
                           detection.confidence > 50);
    
    last_detection = detection;
    return detection;
}

inline bool BeanDetector::isBeanPresent(uint8_t sensor_id) {
    BeanDetection detection = detectBean(sensor_id);
    return detection.is_present;
}

inline BeanType BeanDetector::identifyBeanType(uint8_t sensor_id) {
    BeanDetection detection = detectBean(sensor_id);
    return detection.type;
}

// ============================================
// CLASIFICACIÓN
// ============================================

inline BeanType BeanDetector::classifyColorToBean(uint16_t r, uint16_t g, uint16_t b) {
    // Normalizar valores a 0-255 si es necesario
    // Los sensores TCS34725 devuelven valores de 16 bits
    r = map(r, 0, 65535, 0, 255);
    g = map(g, 0, 65535, 0, 255);
    b = map(b, 0, 65535, 0, 255);
    
    Serial.printf("RGB: R=%d G=%d B=%d -> ", r, g, b);
    
    // Clasificación basada en dominancia de color
    // Estos umbrales deben calibrarse en campo
    
    // NEGRO/AZUL (Overripe)
    if(r < 100 && g < 100 && b < 100) {
        Serial.println("BLACK (Overripe)");
        return BeanType::OVERRIPE;
    }
    if(b > r && b > g && b > 150) {
        Serial.println("BLUE (Overripe)");
        return BeanType::OVERRIPE;
    }
    
    // VERDE (Green - no recolectar)
    if(g > r && g > b && g > 150) {
        Serial.println("GREEN (Don't collect)");
        return BeanType::GREEN;
    }
    
    // ROJO (Ripe)
    if(r > 200 && r > g * 1.5 && r > b * 1.5) {
        Serial.println("RED (Ripe)");
        return BeanType::RIPE;
    }
    
    // NARANJA (Ripe)
    if(r > 180 && g > 100 && g < r && b < 100) {
        Serial.println("ORANGE (Ripe)");
        return BeanType::RIPE;
    }
    
    // AMARILLO (Ripe)
    if(r > 180 && g > 180 && b < 150) {
        Serial.println("YELLOW (Ripe)");
        return BeanType::RIPE;
    }
    
    Serial.println("UNKNOWN");
    return BeanType::UNKNOWN;
}

inline uint16_t BeanDetector::calculateConfidence(uint16_t r, uint16_t g, uint16_t b, 
                                                   BeanType type) {
    // Normalizar
    r = map(r, 0, 65535, 0, 255);
    g = map(g, 0, 65535, 0, 255);
    b = map(b, 0, 65535, 0, 255);
    
    uint16_t confidence = 0;
    
    switch(type) {
        case BeanType::RIPE:
            // Confianza basada en cuán dominante es el rojo
            if(r > 200) confidence = 90;
            else if(r > 150) confidence = 70;
            else confidence = 50;
            break;
            
        case BeanType::OVERRIPE:
            // Confianza basada en cuán oscuro o azul es
            if(b > 150 || (r < 80 && g < 80 && b < 80)) confidence = 85;
            else confidence = 60;
            break;
            
        case BeanType::GREEN:
            // Confianza basada en dominancia verde
            if(g > 180 && g > r && g > b) confidence = 80;
            else confidence = 55;
            break;
            
        default:
            confidence = 0;
            break;
    }
    
    return confidence;
}

inline bool BeanDetector::isRipe(BeanType type) const {
    return type == BeanType::RIPE;
}

inline bool BeanDetector::isOverripe(BeanType type) const {
    return type == BeanType::OVERRIPE;
}

inline bool BeanDetector::isGreen(BeanType type) const {
    return type == BeanType::GREEN;
}

// ============================================
// MÚLTIPLES SENSORES
// ============================================

inline BeanDetection BeanDetector::scanAllSensors() {
    BeanDetection best_detection;
    best_detection.confidence = 0;
    
    // Escanear los 3 sensores de color
    for(uint8_t i = 0; i < 3; i++) {
        BeanDetection detection = detectBean(i);
        
        // Guardar la detección con mayor confianza
        if(detection.confidence > best_detection.confidence) {
            best_detection = detection;
        }
    }
    
    return best_detection;
}

inline int BeanDetector::countBeansInRange() {
    int count = 0;
    
    for(uint8_t i = 0; i < 3; i++) {
        if(isBeanPresent(i)) {
            count++;
        }
    }
    
    return count;
}

// ============================================
// UTILIDADES
// ============================================

inline const char* BeanDetector::beanTypeToString(BeanType type) const {
    switch(type) {
        case BeanType::NONE: return "NONE";
        case BeanType::GREEN: return "GREEN";
        case BeanType::RIPE: return "RIPE";
        case BeanType::OVERRIPE: return "OVERRIPE";
        case BeanType::UNKNOWN: return "UNKNOWN";
        default: return "ERROR";
    }
}

inline void BeanDetector::printDetection(const BeanDetection& detection) {
    Serial.println("\n=== Bean Detection ===");
    Serial.printf("Sensor:     %d\n", detection.sensor_id);
    Serial.printf("Present:    %s\n", detection.is_present ? "YES" : "NO");
    Serial.printf("Type:       %s\n", beanTypeToString(detection.type));
    Serial.printf("Confidence: %d%%\n", detection.confidence);
    Serial.printf("RGB:        R=%d G=%d B=%d\n", detection.r, detection.g, detection.b);
    
    // Indicar acción recomendada
    if(detection.is_present) {
        if(isRipe(detection.type)) {
            Serial.println("Action:     COLLECT -> Red Container");
        }
        else if(isOverripe(detection.type)) {
            Serial.println("Action:     COLLECT -> Blue Container");
        }
        else if(isGreen(detection.type)) {
            Serial.println("Action:     LEAVE on tree");
        }
        else {
            Serial.println("Action:     UNCERTAIN - skip");
        }
    }
    Serial.println("====================\n");
}

#endif // BEAN_DETECTOR_H