#include "route_manager.h"

RouteManager routeManager;

void RouteManager::init() {
    currentTree = 0;
}

void RouteManager::exitStartingBox() {
    Serial.println("Saliendo del cuadro de inicio");
    
    // Avanzar hasta salir del cuadro
    movement.moveForwardUntilLine(BASE_SPEED);
    delay(200);
    
    // Pequeño ajuste para quedar completamente fuera
    movement.moveForwardTimed(SLOW_SPEED, 500);
    motors.stop();
    
    Serial.println("Fuera del cuadro de inicio");
}

void RouteManager::moveToRightBoundary() {
    Serial.println("Moviéndose al límite derecho");
    
    // Moverse a la derecha siguiendo la línea hasta encontrar el borde
    lineFollowing.followLineRight(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En límite derecho");
}

void RouteManager::moveToTreeZone() {
    Serial.println("Avanzando hacia zona de árboles");
    
    // Verificar si hay paso libre entre pools
    while (!isPathClear()) {
        Serial.println("alberca");
        //ir hacia la izquierda hasta que esté libre
        //implementar rodear obstáculo
        delay(100);
        sensors.updateAll();
    }
    
    // Avanzar hasta la línea de los árboles
    lineFollowing.followLineForward(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En zona de árboles");
}

void RouteManager::adjustPositionForTree(int treeNumber) {
    // Calcular distancia lateral según número de árbol
    // Árbol 1: más a la derecha
    // Árbol 2: centro
    // Árbol 3: más a la izquierda
    
    unsigned long lateralTime = 0;
    
    switch (treeNumber) {
        case 1:
            lateralTime = 500; // Poco desplazamiento
            break;
        case 2:
            lateralTime = 2000; // Desplazamiento medio
            break;
        case 3:
            lateralTime = 3500; // Mayor desplazamiento
            break;
    }
    
    if (lateralTime > 0) {
        movement.moveLeftTimed(BASE_SPEED, lateralTime);
        delay(200);
    }
}

void RouteManager::alignWithTree(int treeNumber) {
    Serial.print("Alineándose con árbol ");
    Serial.println(treeNumber);
    
    currentTree = treeNumber;
    
    // Moverse lateralmente a la posición del árbol
    adjustPositionForTree(treeNumber);
    
    // Avanzar un poco más hacia el árbol
    movement.moveForwardTimed(SLOW_SPEED, 1000);
    delay(200);
    
    motors.stop();
    Serial.println("Alineado con árbol");
}

bool RouteManager::isPathClear() {
    sensors.updateAll();
    return sensors.detectOpenPathBetweenPools();
}

void RouteManager::navigateAroundPools() {
    // Si no hay paso directo, intentar navegar alrededor
    Serial.println("Navegando alrededor de obstáculos");
    
    // Estrategia simple: moverse lateralmente
    movement.moveLeftTimed(BASE_SPEED, 1000);
    delay(200);
    
    movement.moveForwardTimed(BASE_SPEED, 2000);
    delay(200);
    
    movement.moveRightTimed(BASE_SPEED, 1000);
    delay(200);
}

void RouteManager::returnToProcessingFacility() {
    Serial.println("Regresando a instalación de procesamiento");
    
    // Retroceder un poco del árbol
    movement.moveBackwardTimed(SLOW_SPEED, 1000);
    delay(200);
    
    // Moverse a la derecha hasta el límite
    lineFollowing.followLineRight(BASE_SPEED);
    delay(200);
    
    // Verificar paso libre
    while (!isPathClear()) {
        Serial.println("Esperando paso libre para regresar...");
        delay(100);
        sensors.updateAll();
    }
    
    // Retroceder hasta la zona de procesamiento
    lineFollowing.followLineBackward(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("De regreso en zona de procesamiento");
}

void RouteManager::positionForDeposit() {
    Serial.println("Posicionándose para depositar granos");
    
    // Ajustes finos para quedar frente a los contenedores
    movement.moveLeftTimed(ALIGN_SPEED, 1500);
    delay(200);
    
    movement.moveForwardTimed(ALIGN_SPEED, 500);
    delay(200);
    
    motors.stop();
    Serial.println("Posicionado para depósito");
}

void RouteManager::executeTreeRoutine(int treeNumber) {
    Serial.println("=================================");
    Serial.print("Ejecutando rutina para árbol ");
    Serial.println(treeNumber);
    Serial.println("=================================");
    
    // 1. Salir del cuadro (solo la primera vez)
    if (treeNumber == 1) {
        exitStartingBox();
        delay(500);
    }
    
    // 2. Ir al límite derecho
    moveToRightBoundary();
    delay(500);
    
    // 3. Avanzar a zona de árboles
    moveToTreeZone();
    delay(500);
    
    // 4. Alinearse con el árbol específico
    alignWithTree(treeNumber);
    delay(500);
    
    // 5. Recolectar granos
    beanCollection.collectAllBeansFromTree();
    delay(1000);
    
    // 6. Regresar a zona de procesamiento
    returnToProcessingFacility();
    delay(500);
    
    // 7. Posicionarse para depositar
    positionForDeposit();
    delay(500);
    
    // 8. Depositar granos
    beanCollection.depositAllBeans();
    delay(1000);
    
    Serial.println("Rutina de árbol completada");
    Serial.println("=================================");
}