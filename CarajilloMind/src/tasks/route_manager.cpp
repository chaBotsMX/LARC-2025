#include "route_manager.h"

RouteManager routeManager;

void RouteManager::init() {
    currentLine = 0;
}

void RouteManager::exitStartingBox() {
    Serial.println("Saliendo del cuadro de inicio");
    
    // Avanzar hasta salir del cuadro
    movement.moveForwardUntilBackLine(BASE_SPEED);
    delay(200);
    
    motors.stop();
    
    Serial.println("Fuera del cuadro de inicio");
}

void RouteManager::moveToRightBoundary() {
    Serial.println("Moviéndose al límite derecho");
    
    // Moverse a la derecha siguiendo la línea hasta encontrar el borde
    movement.moveRightUntilRightLineFollowingLine(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En límite derecho");
}

void RouteManager::moveToTreeZone() {
    Serial.println("Avanzando hacia zona de árboles");
    
    // Verificar si hay paso libre entre pools
    while (!isPathClearForward()) {
        Serial.println("alberca");
        //implementar rodear obstáculo
        routeManager.navigateAroundPoolForward();
        delay(100);
        sensors.updateAll();
    }
    
    // Avanzar hasta la línea de los árboles
    movement.moveForwardUntilFrontLineFollowingLine(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En zona de árboles");
}

void RouteManager::collectLine(int lineNumber) {
    Serial.print("Recolectando línea ");
    Serial.println(lineNumber);
    
    currentLine = lineNumber;
    
    //Moverte al siguiente grano de café
    int iterations = (lineNumber == 1) ? 15 : (lineNumber == 2) ? 18 : 15;
    for (int i = 0; i < iterations; i++) {
        movement.moveToNextBean(BASE_SPEED, lineNumber);
        delay(100);
    }
    
    motors.stop();
    Serial.println("Línea recolectada");
}

void RouteManager::moveToLeftBoundary() {
    Serial.println("Moviéndose al límite izquierdo");
    
    // Moverse a la izquierda siguiendo la línea hasta encontrar el borde
    movement.moveLeftUntilLeftLineFollowingLine(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En límite izquierdo");
}

bool RouteManager::isPathClearForward() {
    sensors.updateAll();
    return !sensors.detectObstacleAhead();
}

bool RouteManager::isPathClearBackward() {
    sensors.updateAll();
    return !sensors.detectObstacleBehind();
}

void RouteManager::navigateAroundPoolForward() {
    // Si no hay paso directo, intentar navegar alrededor
    Serial.println("Navegando alrededor de obstáculos");
    
    movement.moveLeftUntilObstacleClear(BASE_SPEED);
    delay(200);

    movement.moveForwardUntilObstacleClear(BASE_SPEED);
    delay(200);
    
    movement.moveRightUntilRightLine(BASE_SPEED);
    delay(200);
}

void RouteManager::navigateAroundPoolBackward() {
    // Si no hay paso directo, intentar navegar alrededor
    Serial.println("Navegando alrededor de obstáculos (retrocediendo)");
    
    movement.moveRightUntilObstacleClear(BASE_SPEED);
    delay(200);

    movement.moveBackwardUntilObstacleClear(BASE_SPEED);
    delay(200);
    
    movement.moveLeftUntilLeftLine(BASE_SPEED);
    delay(200);
}


void RouteManager::returnToProcessingFacility() {
    Serial.println("Regresando a instalación de procesamiento");

    // Verificar si hay paso libre entre pools
    while (!isPathClearBackward()) {
        Serial.println("alberca");
        //implementar rodear obstáculo
        routeManager.navigateAroundPoolBackward();
        delay(100);
        sensors.updateAll();
    }
    
    // Avanzar hasta la línea de inicio
    movement.moveBackwardUntilBackLineFollowingLine(BASE_SPEED);
    delay(200);
    
    motors.stop();
    Serial.println("En zona de procesamiento");
    
}

void RouteManager::depositBeans() {
    Serial.println("Iniciando el depósito de granos");

    movement.moveToDepositBox(ALIGN_SPEED, 1);
    delay(200);

    movement.moveToDepositBox(ALIGN_SPEED, 2);
    delay(200);
    
    motors.stop();
    Serial.println("Posicionado para depósito");
}

void RouteManager::executeLineRoutine(int lineNumber) {
    Serial.println("=================================");
    Serial.print("Ejecutando rutina para línea ");
    Serial.println(lineNumber);
    Serial.println("=================================");
    
    // 1. Salir del cuadro (solo la primera vez)
    if (lineNumber == 1) {
        exitStartingBox();
        delay(500);
    }
    
    // 2. Ir al límite derecho
    moveToRightBoundary();
    delay(500);
    
    // 3. Avanzar a zona de árboles
    moveToTreeZone();
    delay(500);
    
    // 4. Recolectar línea de árboles
    collectLine(lineNumber);
    delay(500);
    
    // 5. Moverse al limite izquierdo
    moveToLeftBoundary();
    delay(1000);
    
    // 6. Regresar a zona de procesamiento
    returnToProcessingFacility();
    delay(500);
    
    // 7. Posicionarse para depositar
    depositBeans();
    delay(500);
    
    Serial.println("Rutina de línea completada");
    Serial.println("=================================");
}