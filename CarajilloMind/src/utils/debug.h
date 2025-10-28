#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>
#include "../hardware/sensors.h"
#include "../config.h"

class Debug {
public:
    void init();
    void printSensorData();
    void printLineSensors();
    void printDistanceSensors();
    void printIMU();
    void printColorSensor(int sensorNum);
    void printSystemStatus();
    
private:
    void printDivider();
};

extern Debug debug;

#endif