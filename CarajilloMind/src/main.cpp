#include <Arduino.h>
#include "config.h"
#include "hardware/motors.h"
#include "hardware/sensors.h"
#include "hardware/mechanisms.h"
#include "navigation/movement.h"
#include "navigation/line_following.h"
#include "tasks/bean_collection.h"
#include "tasks/route_manager.h"
#include "utils/debug.h"

// Variables globales
RobotState currentState = INIT;
int currentTreeNumber = 1;
bool buttonPressed = false;
unsigned long startTime = 0;
const unsigned long MAX_RUNTIME = 500000; // 8 minutos en milisegundos

void setup() {
    // Inicializar debug primero
    debug.init();
    Serial.println("Carajillo - chaBots Espresso");
    Serial.println("Inicializando");
    
    // Inicializar hardware
    motors.init();
    Serial.println("✓ Motores inicializados");
    
    sensors.init();
    Serial.println("✓ Sensores inicializados");
    
    mechanisms.init();
    Serial.println("✓ Mecanismos inicializados");
    
    // Inicializar navegación
    movement.init();
    lineFollowing.init();
    Serial.println("✓ Sistema de navegación inicializado");
    
    // Inicializar tareas
    beanCollection.init();
    routeManager.init();
    Serial.println("✓ Gestión de tareas inicializada");
    
    // Configurar botón de inicio
    pinMode(START_BUTTON, INPUT_PULLUP);
    
    Serial.println("Sistema listo. Esperando botón de inicio...");
    //debug.printDivider();

    // Configurar ganancias PID si quieres valores personalizados
    lineFollowing.setPIDGains(2.5, 0.15, 0.8);  // Ajustar según pruebas
    movement.setHeadingPIDGains(2.0, 0.05, 0.3);
    
    Serial.println("✓ PID configurado");
}

void waitForStart() {
    delay(5000); // Esperar 5 segundos antes de iniciar
    startTime = millis();
    Serial.println("¡INICIO!");
}

bool isTimeout() {
    return (millis() - startTime) > MAX_RUNTIME;
}

/*
void checkSafetyConditions() {
    sensors.updateAll();
    
    // Verificar límites del campo
    if (sensors.isOutOfBounds()) {
        Serial.println("¡ADVERTENCIA! Robot fuera de límites");
        motors.stop();
        currentState = ERROR;
        return;
    }
    
    // Verificar timeout
    if (isTimeout()) {
        Serial.println("Tiempo máximo alcanzado (8 minutos)");
        currentState = COMPLETE;
        motors.stop();
        return;
    }
}
*/

void executeStateMachine() {
    switch (currentState) {
        case INIT:
            waitForStart();
            currentState = EXIT_BOX;
            break;
            
        case EXIT_BOX:
            Serial.println("ESTADO: Saliendo del cuadro");
            routeManager.exitStartingBox();
            currentState = NAVIGATE_TO_TREE;
            currentTreeNumber = 1;
            break;
            
        case NAVIGATE_TO_TREE:
            Serial.print("ESTADO: Navegando al árbol ");
            Serial.println(currentTreeNumber);
            
            routeManager.moveToRightBoundary();
            delay(300);
            routeManager.moveToTreeZone();
            delay(300);
            
            currentState = ALIGN_WITH_TREE;
            break;
            
        case ALIGN_WITH_TREE:
            Serial.println("ESTADO: Alineándose con árbol");
            routeManager.alignWithTree(currentTreeNumber);
            currentState = COLLECT_BEANS;
            break;
            
        case COLLECT_BEANS:
            Serial.println("ESTADO: Recolectando granos");
            beanCollection.collectAllBeansFromTree();
            currentState = RETURN_TO_BASE;
            break;
            
        case RETURN_TO_BASE:
            Serial.println("ESTADO: Regresando a base");
            routeManager.returnToProcessingFacility();
            currentState = DEPOSIT_BEANS;
            break;
            
        case DEPOSIT_BEANS:
            Serial.println("ESTADO: Depositando granos");
            routeManager.positionForDeposit();
            delay(300);
            beanCollection.depositAllBeans();
            
            // Verificar si hay más árboles
            currentTreeNumber++;
            if (currentTreeNumber <= 3) {
                Serial.print("Preparando para siguiente árbol: ");
                Serial.println(currentTreeNumber);
                currentState = NAVIGATE_TO_TREE;
            } else {
                Serial.println("¡Todos los árboles completados!");
                currentState = COMPLETE;
            }
            break;
            
        case COMPLETE:
            Serial.println("ESTADO: Competencia completada");
            motors.stop();
            debug.printSystemStatus();
            
            Serial.println("\n========== RESUMEN FINAL ==========");
            Serial.print("Granos maduros recolectados: ");
            Serial.println(beanCollection.getRipeBeansCollected());
            Serial.print("Granos sobremaduros recolectados: ");
            Serial.println(beanCollection.getOverripeBeansCollected());
            Serial.print("Tiempo total: ");
            Serial.print((millis() - startTime) / 1000);
            Serial.println(" segundos");
            Serial.println("===================================\n");
            
            // Mantener el robot detenido
            while (true) {
                delay(1000);
            }
            break;
            
        case ERROR:
            Serial.println("ESTADO: Error - Robot detenido");
            motors.stop();
            debug.printSensorData();
            
            // Mantener el robot detenido
            while (true) {
                delay(1000);
            }
            break;
    }
}

void loop() {
    // Ejecutar la máquina de estados
    executeStateMachine();
    
    // Verificar condiciones de seguridad continuamente
    if (currentState != INIT && currentState != COMPLETE && currentState != ERROR) {
        //checkSafetyConditions();
    }
    
    // Pequeño delay para no saturar el procesador
    delay(10);
}