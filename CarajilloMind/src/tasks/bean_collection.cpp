#include "bean_collection.h"

BeanCollection beanCollection;

void BeanCollection::init() {
    ripeCount = 0;
    overripeCount = 0;
}

void BeanCollection::resetCounters() {
    ripeCount = 0;
    overripeCount = 0;
}

BeanType BeanCollection::detectBeanAtPosition() {
    // Leer de varios sensores de color para mayor precisión
    ColorSensorData color1 = sensors.readColorSensor(MUX_COLOR_1);
    delay(50);
    ColorSensorData color2 = sensors.readColorSensor(MUX_COLOR_2);
    
    // Si ambos coinciden, confiamos en la lectura
    if (color1.type == color2.type && color1.type != NONE) {
        return color1.type;
    }
    
    // Si no coinciden, hacer lectura adicional
    ColorSensorData color3 = sensors.readColorSensor(MUX_COLOR_1);
    return color3.type;
}

bool BeanCollection::isBeanPresent() {
    BeanType detected = detectBeanAtPosition();
    return (detected != NONE);
}

void BeanCollection::grabBean() {
    mechanisms.openGripper();
    delay(300);
    mechanisms.extendArm();
    delay(500);
    mechanisms.closeGripper();
    delay(500);
    mechanisms.retractArm();
    delay(500);
}

void BeanCollection::storeBean(BeanType type) {
    // Mover a posición de almacenamiento según tipo
    mechanisms.positionCarouselForBean(type);
    delay(300);
    mechanisms.raiseElevator();
    delay(500);
    mechanisms.openGripper();
    delay(300);
    mechanisms.lowerElevator();
    delay(500);
    
    // Actualizar contadores
    if (type == RIPE) {
        ripeCount++;
    } else if (type == OVERRIPE) {
        overripeCount++;
    }
    
    Serial.print("Granos recolectados - Maduros: ");
    Serial.print(ripeCount);
    Serial.print(", Sobremaduros: ");
    Serial.println(overripeCount);
}

void BeanCollection::executePickSequence(BeanType type) {
    if (type == GREEN) {
        Serial.println("Grano verde detectado - Ignorando");
        return; // No recolectar granos verdes
    }
    
    Serial.print("Recolectando grano tipo: ");
    Serial.println(type);
    
    grabBean();
    storeBean(type);
}

void BeanCollection::waitForBeanInPosition() {
    unsigned long timeout = 2000;
    unsigned long startTime = millis();
    
    while (!isBeanPresent() && (millis() - startTime < timeout)) {
        delay(50);
    }
}

void BeanCollection::collectFromLevel(TreeLevel level) {
    Serial.print("Recolectando nivel: ");
    Serial.println(level);
    
    // Posicionar brazo para el nivel
    mechanisms.positionArmForLevel(level);
    delay(500);
    
    // Número de granos por nivel (según las reglas)
    int beansInLevel = (level == MIDDLE) ? 6 : 5;
    
    for (int i = 0; i < beansInLevel; i++) {
        // Detectar tipo de grano
        waitForBeanInPosition();
        BeanType type = detectBeanAtPosition();
        
        if (type != NONE) {
            executePickSequence(type);
        }
        
        // Mover a siguiente posición (usando stepper)
        mechanisms.moveStepper(200, true); // 200 pasos por posición
        delay(500);
    }
}

void BeanCollection::collectAllBeansFromTree() {
    Serial.println("Iniciando recolección de árbol completo");
    
    // Recolectar desde nivel bajo
    collectFromLevel(LOWER);
    delay(500);
    
    // Recolectar desde nivel medio
    collectFromLevel(MIDDLE);
    delay(500);
    
    // Recolectar desde nivel alto
    collectFromLevel(UPPER);
    delay(500);
    
    Serial.println("Recolección completada");
}

void BeanCollection::depositAllBeans() {
    Serial.println("Depositando granos en contenedores");
    
    // Esta función asume que el robot ya está posicionado frente a los contenedores
    // Depositar granos maduros en contenedor rojo
    mechanisms.positionCarouselForBean(RIPE);
    delay(500);
    // Aquí iría la secuencia para vaciar el contenedor interno
    // Por simplicidad, asumimos que el carrusel libera los granos
    
    delay(1000);
    
    // Depositar granos sobremaduros en contenedor azul
    mechanisms.positionCarouselForBean(OVERRIPE);
    delay(500);
    
    delay(1000);
    
    Serial.println("Depósito completado");
}

int BeanCollection::getRipeBeansCollected() {
    return ripeCount;
}

int BeanCollection::getOverripeBeansCollected() {
    return overripeCount;
}