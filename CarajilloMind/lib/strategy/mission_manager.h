#ifndef MISSION_MANAGER_H
#define MISSION_MANAGER_H

#include <Arduino.h>
#include "../navigation/movement.h"
#include "../navigation/line_follower.h"
#include "../navigation/obstacle_avoidance.h"
#include "../collection/arm_control.h"
#include "../collection/carousel.h"
#include "../config/constants.h"

// ============================================
// CLASE GESTORA DE MISIÓN
// ============================================

class MissionManager {
public:
    MissionManager(Movement& movement, 
                   LineFollower& line_follower,
                   ObstacleAvoidance& obstacle_avoidance,
                   ArmControl& arm_control,
                   CarouselController& carousel);
    
    // Inicialización
    void begin();
    
    // Misión principal
    void executeFullMission();
    
    // Fases de la misión (modulares y reutilizables)
    void navigateToTree(uint8_t tree_number);
    void harvestTree(uint8_t tree_number);
    void returnToProcessingFacility();
    void depositAllBeans();
    
    // Subfases reutilizables
    void harvestLevel(uint8_t level);
    bool navigateThroughCentralZone();
    bool alignWithTreeZone(uint8_t tree_number);
    
    // Estado
    MissionState getCurrentState() const;
    void printMissionStatus();
    
private:
    // Referencias a los módulos
    Movement& movement;
    LineFollower& line_follower;
    ObstacleAvoidance& obstacle_avoidance;
    ArmControl& arm_control;
    CarouselController& carousel;
    
    // Estado de la misión
    MissionState current_state;
    uint8_t current_tree;
    unsigned long mission_start_time;
    unsigned long mission_elapsed_time;
    
    // Helpers
    void transitionTo(MissionState new_state);
    bool checkTimeout();
    void handleError(const char* error_msg);
};

// ============================================
// IMPLEMENTACIÓN
// ============================================

inline MissionManager::MissionManager(Movement& movement,
                                       LineFollower& line_follower,
                                       ObstacleAvoidance& obstacle_avoidance,
                                       ArmControl& arm_control,
                                       CarouselController& carousel)
    : movement(movement),
      line_follower(line_follower),
      obstacle_avoidance(obstacle_avoidance),
      arm_control(arm_control),
      carousel(carousel),
      current_state(MissionState::INIT),
      current_tree(0),
      mission_start_time(0),
      mission_elapsed_time(0)
{}

inline void MissionManager::begin() {
    Serial.println("\n");
    Serial.println("================================");
    Serial.println("  CARAJILLO - MISSION MANAGER  ");
    Serial.println("     ChaBots Espresso Team     ");
    Serial.println("================================\n");
    
    current_state = MissionState::INIT;
    current_tree = 0;
    mission_start_time = millis();
    
    Serial.println("Mission Manager initialized");
    Serial.println("Ready to start mission!\n");
}

// ============================================
// MISIÓN PRINCIPAL
// ============================================

inline void MissionManager::executeFullMission() {
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║   STARTING FULL MISSION SEQUENCE   ║");
    Serial.println("╚════════════════════════════════════╝\n");
    
    mission_start_time = millis();
    
    // Repetir para los 3 árboles
    for(uint8_t tree = 1; tree <= Tree::NUM_TREES; tree++) {
        current_tree = tree;
        
        Serial.printf("\n\n████████ TREE %d / %d ████████\n\n", tree, Tree::NUM_TREES);
        
        // Fase 1: Navegar al árbol
        transitionTo(static_cast<MissionState>(
            static_cast<int>(MissionState::NAVIGATE_TO_TREE_1) + (tree - 1) * 2));
        navigateToTree(tree);
        
        // Verificar timeout
        if(checkTimeout()) {
            handleError("Mission timeout!");
            break;
        }
        
        // Fase 2: Cosechar el árbol
        transitionTo(static_cast<MissionState>(
            static_cast<int>(MissionState::HARVEST_TREE_1) + (tree - 1) * 2));
        harvestTree(tree);
        
        // Verificar timeout
        if(checkTimeout()) {
            handleError("Mission timeout!");
            break;
        }
        
        // Imprimir progreso
        carousel.printInventory();
    }
    
    // Fase 3: Regresar a la zona de procesamiento
    Serial.println("\n\n████████ RETURNING TO FACILITY ████████\n");
    transitionTo(MissionState::RETURN_TO_FACILITY);
    returnToProcessingFacility();
    
    // Fase 4: Depositar todos los granos
    Serial.println("\n\n████████ DEPOSITING BEANS ████████\n");
    transitionTo(MissionState::DEPOSIT_BEANS);
    depositAllBeans();
    
    // Misión completada
    transitionTo(MissionState::MISSION_COMPLETE);
    
    mission_elapsed_time = millis() - mission_start_time;
    
    Serial.println("\n╔════════════════════════════════════╗");
    Serial.println("║      MISSION COMPLETE!             ║");
    Serial.println("╚════════════════════════════════════╝\n");
    
    Serial.printf("Total time: %lu seconds\n", mission_elapsed_time / 1000);
    carousel.printInventory();
}

// ============================================
// NAVEGACIÓN A ÁRBOL
// ============================================

inline void MissionManager::navigateToTree(uint8_t tree_number) {
    Serial.printf(">>> Navigating to Tree %d...\n", tree_number);
    
    // 1. Salir de la zona de procesamiento siguiendo la línea
    Serial.println("  [1/4] Exiting processing facility...");
    line_follower.followLineDistance(700, Speed::CRUISE_SPEED);
    delay(200);
    
    // 2. Navegar por la zona central (evitando piscinas)
    Serial.println("  [2/4] Navigating through central zone...");
    if(!navigateThroughCentralZone()) {
        handleError("Failed to navigate through central zone");
        return;
    }
    
    // 3. Alinearse con el sector del árbol específico
    Serial.println("  [3/4] Aligning with tree zone...");
    if(!alignWithTreeZone(tree_number)) {
        handleError("Failed to align with tree zone");
        return;
    }
    
    // 4. Aproximarse al árbol
    Serial.println("  [4/4] Approaching tree...");
    line_follower.followLineDistance(300, Speed::SLOW_SPEED);
    
    // Alineación final con el árbol
    delay(300);
    obstacle_avoidance.alignWithTree();
    
    Serial.printf(">>> Arrived at Tree %d!\n\n", tree_number);
}

// ============================================
// COSECHA DE ÁRBOL
// ============================================

inline void MissionManager::harvestTree(uint8_t tree_number) {
    Serial.printf(">>> Harvesting Tree %d...\n", tree_number);
    
    int beans_from_tree = 0;
    
    // Cosechar los 3 niveles del árbol
    for(uint8_t level = 1; level <= Tree::NUM_LEVELS; level++) {
        Serial.printf("\n  ═══ Level %d ═══\n", level);
        
        harvestLevel(level);
        
        // Pequeña pausa entre niveles
        delay(500);
    }
    
    beans_from_tree = carousel.getTotalBeansCollected();
    
    Serial.printf("\n>>> Tree %d harvest complete! Collected %d beans\n\n", 
                 tree_number, beans_from_tree);
}

inline void MissionManager::harvestLevel(uint8_t level) {
    // Cantidad esperada de granos en este nivel
    int expected_beans = Tree::BEANS_PER_LEVEL[level - 1];
    
    Serial.printf("  Expected beans at level %d: %d\n", level, expected_beans);
    
    // Extender brazo al nivel
    arm_control.extendToLevel(level);
    delay(500);
    
    // Intentar recolectar cada grano en el nivel
    // En el campo real, esto podría ser un barrido lateral
    // Por ahora, intentamos posiciones fijas
    
    int beans_collected_this_level = 0;
    int attempts = 0;
    const int MAX_ATTEMPTS = expected_beans + 2; // Margen extra
    
    while(beans_collected_this_level < expected_beans && attempts < MAX_ATTEMPTS) {
        attempts++;
        
        Serial.printf("    Attempt %d/%d...\n", attempts, MAX_ATTEMPTS);
        
        // Intentar recolectar grano en posición actual
        bool success = arm_control.collectBeanAtLevel(level);
        
        if(success) {
            beans_collected_this_level++;
            
            // Obtener tipo de grano para clasificar
            BeanDetection detection = arm_control.bean_detector.detectBean();
            
            // Transferir al carrusel
            carousel.receiveBean(detection.type);
            carousel.sortAndStore(detection.type);
            
            // Liberar grano
            arm_control.releaseBean();
            
            Serial.printf("    ✓ Bean collected! (%d/%d)\n", 
                         beans_collected_this_level, expected_beans);
        }
        
        // Moverse lateralmente para el siguiente grano
        if(beans_collected_this_level < expected_beans) {
            movement.strafeRight(60, Speed::SLOW_SPEED); // ~6cm entre granos
            delay(300);
        }
    }
    
    // Retraer brazo
    arm_control.retract();
    
    Serial.printf("  Level %d complete: %d beans collected\n", 
                 level, beans_collected_this_level);
}

// ============================================
// NAVEGACIÓN ZONA CENTRAL (PISCINAS)
// ============================================

inline bool MissionManager::navigateThroughCentralZone() {
    Serial.println("    → Entering central zone with pools...");
    
    // Avanzar lentamente detectando apertura entre piscinas
    unsigned long start_time = millis();
    unsigned long timeout = 15000; // 15 segundos
    
    while(millis() - start_time < timeout) {
        // Actualizar sensores
        obstacle_avoidance.checkForObstacles();
        
        // Buscar apertura entre piscinas
        if(obstacle_avoidance.detectOpenPathBetweenPools()) {
            Serial.println("    → Open path found between pools!");
            
            // Navegar a través de la apertura
            bool success = obstacle_avoidance.navigateThroughPools(Speed::SLOW_SPEED);
            
            if(success) {
                Serial.println("    → Successfully crossed central zone!");
                return true;
            }
        }
        
        // Moverse lentamente siguiendo la línea
        line_follower.followLine(Speed::SLOW_SPEED);
        delay(50);
    }
    
    Serial.println("    ✗ Timeout navigating central zone");
    return false;
}

inline bool MissionManager::alignWithTreeZone(uint8_t tree_number) {
    Serial.printf("    → Aligning with tree zone %d...\n", tree_number);
    
    // Los 3 árboles están en sectores separados
    // Tree 1 = sector izquierdo
    // Tree 2 = sector central  
    // Tree 3 = sector derecho
    
    // Seguir línea hasta llegar al sector correcto
    float distance_to_sector = Field::TREE_SECTOR_WIDTH_MM * (tree_number - 1);
    
    if(tree_number > 1) {
        // Moverse lateralmente al sector correcto
        movement.strafeRight(distance_to_sector, Speed::CRUISE_SPEED);
        delay(500);
    }
    
    // Seguir línea hacia el árbol
    line_follower.followLineDistance(400, Speed::CRUISE_SPEED);
    
    Serial.println("    → Aligned with tree zone!");
    return true;
}

// ============================================
// RETORNO Y DEPÓSITO
// ============================================

inline void MissionManager::returnToProcessingFacility() {
    Serial.println(">>> Returning to processing facility...\n");
    
    // 1. Retroceder del árbol
    Serial.println("  [1/3] Backing away from tree...");
    movement.moveBackward(400, Speed::CRUISE_SPEED);
    delay(300);
    
    // 2. Navegar de regreso por zona central
    Serial.println("  [2/3] Crossing central zone...");
    movement.rotateTo(180); // Girar 180° para regresar
    delay(300);
    
    if(!navigateThroughCentralZone()) {
        handleError("Failed to return through central zone");
        return;
    }
    
    // 3. Entrar a la zona de procesamiento
    Serial.println("  [3/3] Entering processing facility...");
    line_follower.followLineDistance(700, Speed::CRUISE_SPEED);
    
    Serial.println(">>> Arrived at processing facility!\n");
}

inline void MissionManager::depositAllBeans() {
    Serial.println(">>> Depositing all collected beans...\n");
    
    BeanInventory& inventory = carousel.getInventory();
    
    Serial.printf("  Total beans to deposit: %d\n", inventory.total_collected);
    Serial.printf("    - Ripe (→ RED):     %d beans\n", inventory.ripe_count);
    Serial.printf("    - Overripe (→ BLUE): %d beans\n\n", inventory.overripe_count);
    
    // Nota: En el código real, necesitarías posicionar el robot
    // frente a cada contenedor y liberar los granos del carrusel
    
    // Posicionarse frente al contenedor ROJO
    Serial.println("  [1/2] Positioning at RED container...");
    // movement.navigateToPosition(RED_CONTAINER_POSITION);
    line_follower.followLineDistance(200, Speed::SLOW_SPEED);
    
    // Depositar granos maduros (ripe)
    Serial.printf("  Depositing %d RIPE beans...\n", inventory.ripe_count);
    for(int i = 0; i < inventory.ripe_count; i++) {
        carousel.depositToContainer(BeanType::RIPE);
        delay(500);
    }
    
    // Moverse al contenedor AZUL
    Serial.println("\n  [2/2] Positioning at BLUE container...");
    movement.strafeRight(450, Speed::SLOW_SPEED); // Distancia entre contenedores
    
    // Depositar granos sobre-maduros (overripe)
    Serial.printf("  Depositing %d OVERRIPE beans...\n", inventory.overripe_count);
    for(int i = 0; i < inventory.overripe_count; i++) {
        carousel.depositToContainer(BeanType::OVERRIPE);
        delay(500);
    }
    
    Serial.println("\n>>> All beans deposited successfully!\n");
    
    // Calcular puntuación final
    int final_score = inventory.ripe_count * Scoring::RIPE_DEPOSITED +
                     inventory.overripe_count * Scoring::OVERRIPE_DEPOSITED;
    
    if(inventory.green_count > 0) {
        final_score += inventory.green_count * Scoring::PENALTY_GREEN;
        Serial.printf("  ⚠ WARNING: %d green beans collected (penalty applied)\n", 
                     inventory.green_count);
    }
    
    Serial.printf("\n  ╔═══════════════════════╗\n");
    Serial.printf("  ║  FINAL SCORE: %3d pts ║\n", final_score);
    Serial.printf("  ╚═══════════════════════╝\n\n");
}

// ============================================
// GESTIÓN DE ESTADO
// ============================================

inline void MissionManager::transitionTo(MissionState new_state) {
    current_state = new_state;
    
    Serial.print("STATE: ");
    switch(current_state) {
        case MissionState::INIT: Serial.println("INIT"); break;
        case MissionState::NAVIGATE_TO_TREE_1: Serial.println("NAVIGATE_TO_TREE_1"); break;
        case MissionState::HARVEST_TREE_1: Serial.println("HARVEST_TREE_1"); break;
        case MissionState::NAVIGATE_TO_TREE_2: Serial.println("NAVIGATE_TO_TREE_2"); break;
        case MissionState::HARVEST_TREE_2: Serial.println("HARVEST_TREE_2"); break;
        case MissionState::NAVIGATE_TO_TREE_3: Serial.println("NAVIGATE_TO_TREE_3"); break;
        case MissionState::HARVEST_TREE_3: Serial.println("HARVEST_TREE_3"); break;
        case MissionState::RETURN_TO_FACILITY: Serial.println("RETURN_TO_FACILITY"); break;
        case MissionState::DEPOSIT_BEANS: Serial.println("DEPOSIT_BEANS"); break;
        case MissionState::MISSION_COMPLETE: Serial.println("MISSION_COMPLETE"); break;
        case MissionState::ERROR: Serial.println("ERROR"); break;
    }
}

inline bool MissionManager::checkTimeout() {
    mission_elapsed_time = millis() - mission_start_time;
    
    if(mission_elapsed_time >= Speed::MAX_ROUND_TIME_MS) {
        Serial.println("\n⚠ MISSION TIMEOUT REACHED! (8 minutes)\n");
        return true;
    }
    
    // Imprimir tiempo restante cada minuto
    unsigned long remaining = Speed::MAX_ROUND_TIME_MS - mission_elapsed_time;
    static unsigned long last_print = 0;
    
    if(millis() - last_print > 60000) {
        Serial.printf("⏱ Time remaining: %lu seconds\n", remaining / 1000);
        last_print = millis();
    }
    
    return false;
}

inline void MissionManager::handleError(const char* error_msg) {
    Serial.printf("\n❌ ERROR: %s\n", error_msg);
    transitionTo(MissionState::ERROR);
    movement.emergencyStop();
}

inline MissionState MissionManager::getCurrentState() const {
    return current_state;
}

inline void MissionManager::printMissionStatus() {
    Serial.println("\n╔═══════════════════════════════════╗");
    Serial.println("║       MISSION STATUS              ║");
    Serial.println("╠═══════════════════════════════════╣");
    Serial.printf("║ State:        %-20s║\n", "Current");
    Serial.printf("║ Tree:         %d / %d               ║\n", current_tree, Tree::NUM_TREES);
    Serial.printf("║ Time elapsed: %lu sec            ║\n", mission_elapsed_time / 1000);
    Serial.printf("║ Beans:        %d collected        ║\n", carousel.getTotalBeansCollected());
    Serial.println("╚═══════════════════════════════════╝\n");
}

#endif // MISSION_MANAGER_H