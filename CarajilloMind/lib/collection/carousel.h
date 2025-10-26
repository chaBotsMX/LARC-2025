#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <Arduino.h>
#include "../drivers/servos.h"
#include "../collection/bean_detector.h"
#include "../config/constants.h"

// ============================================
// ESTRUCTURA DE INVENTARIO
// ============================================

struct BeanInventory {
    int ripe_count;
    int overripe_count;
    int green_count;      // No deberían estar aquí, pero por si acaso
    int unknown_count;
    int total_collected;
    
    BeanInventory() : ripe_count(0), overripe_count(0), 
                     green_count(0), unknown_count(0), total_collected(0) {}
    
    void add(BeanType type) {
        total_collected++;
        switch(type) {
            case BeanType::RIPE: ripe_count++; break;
            case BeanType::OVERRIPE: overripe_count++; break;
            case BeanType::GREEN: green_count++; break;
            default: unknown_count++; break;
        }
    }
    
    int getScore() const {
        // Calcular puntuación según reglas LARC
        int score = 0;
        score += ripe_count * Scoring::RIPE_COLLECTED;
        score += overripe_count * Scoring::OVERRIPE_COLLECTED;
        score += green_count * Scoring::PENALTY_GREEN;  // Penalización
        return score;
    }
    
    void print() const {
        Serial.println("\n=== Bean Inventory ===");
        Serial.printf("Ripe (R/O/Y):  %d beans\n", ripe_count);
        Serial.printf("Overripe (B/K): %d beans\n", overripe_count);
        Serial.printf("Green:          %d beans (ERROR!)\n", green_count);
        Serial.printf("Unknown:        %d beans\n", unknown_count);
        Serial.printf("Total:          %d beans\n", total_collected);
        Serial.printf("Current Score:  %d points\n", getScore());
        Serial.println("====================\n");
    }
};

// ============================================
// CLASE DEL CARRUSEL/CLASIFICADOR
// ============================================

class CarouselController {
public:
    CarouselController(ServoController& servos, BeanDetector& bean_detector);
    
    // Inicialización
    void begin();
    
    // Operaciones principales
    bool receiveBean(BeanType type);
    void sortAndStore(BeanType type);
    void depositToContainer(BeanType type);
    
    // Inventario
    BeanInventory& getInventory();
    int getTotalBeansCollected() const;
    void printInventory();
    
    // Control del sistema
    void openElevator();
    void closeElevator();
    void resetCarousel();
    
private:
    ServoController& servos;
    BeanDetector& bean_detector;
    
    BeanInventory inventory;
    
    // Helpers
    void routeToContainer(BeanType type);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline CarouselController::CarouselController(ServoController& servos,
                                               BeanDetector& bean_detector)
    : servos(servos),
      bean_detector(bean_detector)
{}

inline void CarouselController::begin() {
    // Posición inicial segura
    servos.carouselClose();
    servos.elevatorClose();
    Serial.println("Carousel controller initialized");
}

// ============================================
// OPERACIONES PRINCIPALES
// ============================================

inline bool CarouselController::receiveBean(BeanType type) {
    Serial.printf("Receiving bean type: %s\n", bean_detector.beanTypeToString(type));
    
    // Verificar que el tipo es válido para recolección
    if(type == BeanType::NONE || type == BeanType::UNKNOWN) {
        Serial.println("ERROR: Invalid bean type");
        return false;
    }
    
    // Si es verde, NO debería estar aquí
    if(type == BeanType::GREEN) {
        Serial.println("WARNING: Green bean collected (should not happen!)");
        inventory.add(BeanType::GREEN);
        return false;
    }
    
    // Agregar al inventario
    inventory.add(type);
    
    Serial.printf("Bean received. Total collected: %d\n", inventory.total_collected);
    return true;
}

inline void CarouselController::sortAndStore(BeanType type) {
    Serial.println("Sorting bean...");
    
    // Abrir elevador para recibir el grano
    openElevator();
    delay(500);
    
    // El grano cae al carrusel
    delay(300);
    
    // Cerrar elevador
    closeElevator();
    delay(300);
    
    // Redirigir según el tipo
    routeToContainer(type);
    
    Serial.println("Bean sorted and stored");
}

inline void CarouselController::depositToContainer(BeanType type) {
    Serial.printf("Depositing %s bean to container...\n", 
                 bean_detector.beanTypeToString(type));
    
    routeToContainer(type);
    
    // Esperar a que caiga
    delay(500);
    
    // Volver a posición cerrada
    servos.carouselClose();
    delay(300);
}

inline void CarouselController::routeToContainer(BeanType type) {
    if(bean_detector.isRipe(type)) {
        // RIPE -> Contenedor ROJO
        Serial.println("Routing to RED container (RIPE)");
        servos.carouselToRipe();
    }
    else if(bean_detector.isOverripe(type)) {
        // OVERRIPE -> Contenedor AZUL
        Serial.println("Routing to BLUE container (OVERRIPE)");
        servos.carouselToOverripe();
    }
    else {
        Serial.println("ERROR: Unknown bean type for routing");
        servos.carouselClose();
    }
}

// ============================================
// CONTROL DEL SISTEMA
// ============================================

inline void CarouselController::openElevator() {
    servos.elevatorOpen();
}

inline void CarouselController::closeElevator() {
    servos.elevatorClose();
}

inline void CarouselController::resetCarousel() {
    Serial.println("Resetting carousel system...");
    servos.carouselClose();
    servos.elevatorClose();
    delay(500);
}

// ============================================
// INVENTARIO
// ============================================

inline BeanInventory& CarouselController::getInventory() {
    return inventory;
}

inline int CarouselController::getTotalBeansCollected() const {
    return inventory.total_collected;
}

inline void CarouselController::printInventory() {
    inventory.print();
}

#endif // CAROUSEL_H