#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H

#include "../navigation/movement.h"
#include "../navigation/line_following.h"
#include "../hardware/sensors.h"
#include "bean_collection.h"
#include "../config.h"

class RouteManager {
public:
    void init();
    
    // Secuencia principal
    void executeLineRoutine(int lineNumber);
    
    // Módulos de navegación
    void exitStartingBox();
    void moveToRightBoundary();
    void moveToLeftBoundary();
    void moveToTreeZone();
    void collectLine(int lineNumber);
    void returnToProcessingFacility();
    void depositBeans();
    
    // Navegación con obstáculos
    void navigateAroundPoolForward();
    void navigateAroundPoolBackward();
    bool isPathClearForward();
    bool isPathClearBackward();
    
private:
    int currentLine;
    
};

extern RouteManager routeManager;

#endif