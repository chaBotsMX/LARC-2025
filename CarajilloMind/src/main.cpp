#include <Arduino.h>
#include <Wire.h>

// Configuración
#include "../lib/config/pins.h"
#include "../lib/config/constants.h"
#include "../lib/config/calibration.h"

// Drivers
#include "../lib/drivers/motors.h"
#include "../lib/drivers/sensors.h"
#include "../lib/drivers/imu.h"
#include "../lib/drivers/servos.h"

// Navegación
#include "../lib/navigation/movement.h"
#include "../lib/navigation/line_follower.h"
#include "../lib/navigation/obstacle_avoidance.h"

// Recolección
#include "../lib/collection/bean_detector.h"
#include "../lib/collection/arm_control.h"
#include "../lib/collection/carousel.h"

// Estrategia
#include "../lib/strategy/mission_manager.h"

void setup() {
  initializeHardware();
  calibrateSensors();
  waitForStartButton();
}

void loop() {
  switch(currentState) {
      case MISSION_START:
          executeHarvestMission();
          break;
      case MANUAL_MODE:
          executeManualControl();
          break;
      case EMERGENCY_STOP:
          handleEmergency();
          break;
  }
}

void executeHarvestMission() {
    // para los 3 árboles
    for(int tree = 1; tree <= 3; tree++) {
        navigateToTree(tree);
        harvestTree(tree);
        returnToProcessingFacility();
        depositBeans();
    }
    celebrateVictory();
}