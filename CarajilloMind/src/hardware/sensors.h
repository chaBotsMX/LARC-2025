#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_VL53L0X.h>  // Ya está incluido
#include <Adafruit_BNO055.h>
#include "../config.h"

struct LineSensorData {
    bool frontLeft;
    bool frontRight;
    bool rearLeft;
    bool rearRight;
};

struct DistanceSensorData {
    int frontLeft;
    int frontRight;
    int rearLeft;
    int rearRight;
    //int flipper;
};

struct ColorSensorData {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t c;
    BeanType type;
};

struct IMUData {
    float heading;
    float pitch;
    float roll;
};

class Sensors {
public:
    void init();
    void updateAll();
    
    // Line sensors
    LineSensorData getLineSensors();
    /*
    bool isAtFrontLine();
    bool isAtRearLine();
    bool isAtLeftLine();
    bool isAtRightLine();
    bool isOutOfBounds();
    */
    
    
    // Distance sensors (ToF)
    DistanceSensorData getDistanceSensors();
    bool detectObstacleAhead();
    bool detectObstacleBehind();
    
    // Color sensors
    ColorSensorData readColorSensor(int sensorNumber);
    BeanType identifyBean(ColorSensorData color);
    
    // IMU
    IMUData getIMUData();
    float getHeading();
    
private:
    LineSensorData lineData;
    DistanceSensorData distanceData;
    IMUData imuData;
    
    Adafruit_BNO055 bno;
    
    // Objetos VL53L0X para cada sensor ToF
    Adafruit_VL53L0X tofFL;  // Front Left
    Adafruit_VL53L0X tofFR;  // Front Right
    Adafruit_VL53L0X tofRL;  // Rear Left
    Adafruit_VL53L0X tofRR;  // Rear Right
    
    void selectMuxChannel(uint8_t channel);
    bool readLineSensor(int pin);
    int readToF(uint8_t channel, Adafruit_VL53L0X &sensor);  // MODIFICADO
};

extern Sensors sensors;

#endif