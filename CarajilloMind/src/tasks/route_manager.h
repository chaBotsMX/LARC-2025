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
    void executeTreeRoutine(int treeNumber);
    
    // Módulos de navegación
    void exitStartingBox();
    void moveToRightBoundary();
    void moveToTreeZone();
    void alignWithTree(int treeNumber);
    void returnToProcessingFacility();
    void positionForDeposit();
    
    // Navegación con obstáculos
    void navigateAroundPools();
    bool isPathClear();
    
private:
    int currentTree;
    void adjustPositionForTree(int treeNumber);
};

extern RouteManager routeManager;

#endif