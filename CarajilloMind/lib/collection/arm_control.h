#ifndef ARM_CONTROL_H
#define ARM_CONTROL_H

#include <Arduino.h>
#include "../drivers/servos.h"
#include "../drivers/sensors.h"
#include "../collection/bean_detector.h"
#include "../config/constants.h"

// ============================================
// ENUMERACIONES
// ============================================

enum class ArmState {
    RETRACTED,
    EXTENDING,
    AT_POSITION,
    GRABBING,
    RETRACTING,
    ERROR
};

// ============================================
// CLASE DE CONTROL DEL BRAZO
// ============================================

class ArmControl {
public:
    ArmControl(ServoController& servos, BeanDetector& bean_detector, 
               DistanceSensors& distance_sensors);
    
    // Inicialización
    void begin();
    
    // Movimientos principales
    void retract();
    void extendToLevel(uint8_t level);  // 1=bajo, 2=medio, 3=alto
    bool grabBean();
    void releaseBean();
    
    // Secuencia completa de recolección
    bool collectBeanAtLevel(uint8_t level);
    
    // Verificación
    bool isBeanInGripper();
    bool isAtPosition();
    
    // Estado
    ArmState getState() const;
    uint8_t getCurrentLevel() const;
    
    // Debug
    void printState();
    
private:
    ServoController& servos;
    BeanDetector& bean_detector;
    DistanceSensors& distance_sensors;
    
    ArmState current_state;
    uint8_t current_level;
    bool bean_in_gripper;
    
    // Helpers
    bool waitForArmMovement(uint16_t delay_ms);
    bool verifyBeanPresence();
    void updateState();
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline ArmControl::ArmControl(ServoController& servos, 
                               BeanDetector& bean_detector,
                               DistanceSensors& distance_sensors)
    : servos(servos),
      bean_detector(bean_detector),
      distance_sensors(distance_sensors),
      current_state(ArmState::RETRACTED),
      current_level(0),
      bean_in_gripper(false)
{}

inline void ArmControl::begin() {
    // Mover a posición segura inicial
    servos.moveToSafePosition();
    current_state = ArmState::RETRACTED;
    current_level = 0;
    Serial.println("Arm control initialized");
}

// ============================================
// MOVIMIENTOS PRINCIPALES
// ============================================

inline void ArmControl::retract() {
    Serial.println("Retracting arm...");
    current_state = ArmState::RETRACTING;
    
    servos.armRetract();
    waitForArmMovement(1000);
    
    current_state = ArmState::RETRACTED;
    current_level = 0;
}

inline void ArmControl::extendToLevel(uint8_t level) {
    if(level < 1 || level > 3) {
        Serial.println("ERROR: Invalid level");
        current_state = ArmState::ERROR;
        return;
    }
    
    Serial.printf("Extending arm to level %d...\n", level);
    current_state = ArmState::EXTENDING;
    
    servos.armExtendToLevel(level);
    waitForArmMovement(1500);
    
    current_state = ArmState::AT_POSITION;
    current_level = level;
}

inline bool ArmControl::grabBean() {
    Serial.println("Attempting to grab bean...");
    current_state = ArmState::GRABBING;
    
    // Verificar que hay un grano presente
    if(!verifyBeanPresence()) {
        Serial.println("No bean detected - aborting grab");
        current_state = ArmState::AT_POSITION;
        return false;
    }
    
    // Cerrar pinza
    servos.gripperClose();
    delay(500); // Dar tiempo para cerrar completamente
    
    // Verificar que se agarró el grano
    bean_in_gripper = isBeanInGripper();
    
    if(bean_in_gripper) {
        Serial.println("Bean grabbed successfully!");
        return true;
    } else {
        Serial.println("Failed to grab bean");
        servos.gripperOpen();
        delay(300);
        current_state = ArmState::AT_POSITION;
        return false;
    }
}

inline void ArmControl::releaseBean() {
    Serial.println("Releasing bean...");
    
    servos.gripperOpen();
    delay(500);
    
    bean_in_gripper = false;
}

// ============================================
// SECUENCIA COMPLETA
// ============================================

inline bool ArmControl::collectBeanAtLevel(uint8_t level) {
    Serial.printf("\n=== Collecting bean at level %d ===\n", level);
    
    // 1. Extender brazo al nivel
    extendToLevel(level);
    delay(300);
    
    // 2. Verificar detección del grano
    BeanDetection detection = bean_detector.detectBean();
    bean_detector.printDetection(detection);
    
    if(!detection.is_present) {
        Serial.println("No bean detected at this position");
        retract();
        return false;
    }
    
    // 3. Verificar que NO es verde (no recolectar verdes)
    if(bean_detector.isGreen(detection.type)) {
        Serial.println("GREEN bean detected - SKIPPING");
        retract();
        return false;
    }
    
    // 4. Intentar agarrar el grano
    bool grabbed = grabBean();
    
    if(!grabbed) {
        Serial.println("Failed to grab bean");
        retract();
        return false;
    }
    
    // 5. Retraer brazo con el grano
    delay(200);
    retract();
    
    Serial.println("=== Collection successful! ===\n");
    return true;
}

// ============================================
// VERIFICACIÓN
// ============================================

inline bool ArmControl::verifyBeanPresence() {
    // Dar 3 intentos de detección
    int detections = 0;
    
    for(int i = 0; i < 3; i++) {
        BeanDetection detection = bean_detector.detectBean();
        
        if(detection.is_present && detection.confidence > 50) {
            detections++;
        }
        
        delay(100);
    }
    
    // Al menos 2 de 3 detecciones positivas
    return detections >= 2;
}

inline bool ArmControl::isBeanInGripper() {
    // Método 1: Verificar si la pinza está cerrada
    bool gripper_closed = servos.isGripperClosed();
    
    // Método 2: Verificar distancia con sensor ToF del brazo
    distance_sensors.update();
    uint16_t distance = distance_sensors.getArm();
    bool object_in_range = (distance < 80); // Menos de 8cm
    
    // Método 3: Verificar detección de color
    BeanDetection detection = bean_detector.detectBean();
    bool bean_detected = detection.is_present;
    
    // Combinación de métodos para mayor confiabilidad
    int confidence_votes = 0;
    if(gripper_closed) confidence_votes++;
    if(object_in_range) confidence_votes++;
    if(bean_detected) confidence_votes++;
    
    Serial.printf("Bean in gripper check: Closed=%d, Distance=%d, Detected=%d -> Votes=%d/3\n",
        gripper_closed, distance, bean_detected, confidence_votes);
    
    // Al menos 2 de 3 indicadores positivos
    return confidence_votes >= 2;
}

inline bool ArmControl::isAtPosition() {
    return current_state == ArmState::AT_POSITION;
}

// ============================================
// ESTADO
// ============================================

inline ArmState ArmControl::getState() const {
    return current_state;
}

inline uint8_t ArmControl::getCurrentLevel() const {
    return current_level;
}

inline void ArmControl::updateState() {
    // Actualizar estado basado en sensores
    if(current_state == ArmState::AT_POSITION) {
        if(!bean_detector.isBeanPresent()) {
            // Ya no hay grano en posición
            Serial.println("Bean no longer detected");
        }
    }
}

// ============================================
// UTILIDADES
// ============================================

inline bool ArmControl::waitForArmMovement(uint16_t delay_ms) {
    // Esperar a que los servos completen el movimiento
    // Los servos SG90/MG995 típicamente tardan ~500ms para 60°
    delay(delay_ms);
    return true;
}

inline void ArmControl::printState() {
    Serial.println("\n=== Arm State ===");
    
    Serial.print("State: ");
    switch(current_state) {
        case ArmState::RETRACTED: Serial.println("RETRACTED"); break;
        case ArmState::EXTENDING: Serial.println("EXTENDING"); break;
        case ArmState::AT_POSITION: Serial.println("AT_POSITION"); break;
        case ArmState::GRABBING: Serial.println("GRABBING"); break;
        case ArmState::RETRACTING: Serial.println("RETRACTING"); break;
        case ArmState::ERROR: Serial.println("ERROR"); break;
    }
    
    Serial.printf("Level: %d\n", current_level);
    Serial.printf("Bean in gripper: %s\n", bean_in_gripper ? "YES" : "NO");
    Serial.println("================\n");
}

#endif // ARM_CONTROL_H