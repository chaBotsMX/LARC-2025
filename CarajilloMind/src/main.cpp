#include <Arduino.h>
#include "SensorColor.h"

// put function declarations here:
int myFunction(int, int);

SensorColor sensor;

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