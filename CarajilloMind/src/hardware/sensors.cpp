#include "sensors.h"

Sensors sensors;

void Sensors::init() {
    // Inicializar pines de sensores de línea
    pinMode(LINE_FRONT_LEFT, INPUT);
    pinMode(LINE_FRONT_RIGHT, INPUT);
    pinMode(LINE_REAR_LEFT, INPUT);
    pinMode(LINE_REAR_RIGHT, INPUT);
    
    // Inicializar I2C
    Wire.begin();
    Wire.setClock(400000); // 400kHz
    
    // Inicializar IMU
    if (!bno.begin()) {
        Serial.println("Error: No se pudo inicializar BNO055");
    }
    bno.setExtCrystalUse(true);
    
    delay(100);
}

void Sensors::selectMuxChannel(uint8_t channel) {
    Wire.beginTransmission(I2C_MULTIPLEXER_ADDR);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

void Sensors::updateAll() {
    // Actualizar sensores de línea
    lineData.frontLeft = readLineSensor(LINE_FRONT_LEFT);
    lineData.frontRight = readLineSensor(LINE_FRONT_RIGHT);
    lineData.rearLeft = readLineSensor(LINE_REAR_LEFT);
    lineData.rearRight = readLineSensor(LINE_REAR_RIGHT);
    
    // Actualizar sensores de distancia
    distanceData.frontLeft = readToF(MUX_TOF_FL);
    distanceData.frontRight = readToF(MUX_TOF_FR);
    distanceData.rearLeft = readToF(MUX_TOF_RL);
    distanceData.rearRight = readToF(MUX_TOF_RR);
    
    // Actualizar IMU
    sensors::event_t event;
    bno.getEvent(&event);
    imuData.heading = event.orientation.x;
    imuData.pitch = event.orientation.y;
    imuData.roll = event.orientation.z;
}

bool Sensors::readLineSensor(int pin) {
    int value = analogRead(pin);
    return value < LINE_THRESHOLD; // true si detecta línea negra
}

int Sensors::readToF(uint8_t channel) {
    selectMuxChannel(channel);
    delay(5);
    
    Adafruit_VL53L0X lox;
    if (!lox.begin()) {
        return -1; // Error
    }
    
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    
    if (measure.RangeStatus != 4) {
        return measure.RangeMilliMeter;
    }
    return -1; // Fuera de rango
}

LineSensorData Sensors::getLineSensors() {
    return lineData;
}

bool Sensors::isAtFrontLine() {
    return lineData.frontLeft && lineData.frontRight;
}

bool Sensors::isAtRearLine() {
    return lineData.rearLeft && lineData.rearRight;
}

bool Sensors::isAtLeftLine() {
    return lineData.frontLeft && lineData.rearLeft;
}

bool Sensors::isAtRightLine() {
    return lineData.frontRight && lineData.rearRight;
}

bool Sensors::isOutOfBounds() {
    // Si cualquier sensor detecta línea en el límite
    return isAtFrontLine() || isAtRearLine() || isAtLeftLine() || isAtRightLine();
}

DistanceSensorData Sensors::getDistanceSensors() {
    return distanceData;
}

bool Sensors::detectObstacleAhead() {
    return (distanceData.frontLeft < DISTANCE_OBSTACLE_STOP && distanceData.frontLeft > 0) ||
           (distanceData.frontRight < DISTANCE_OBSTACLE_STOP && distanceData.frontRight > 0);
}

bool Sensors::detectOpenPathBetweenPools() {
    // Si ambos ToF frontales detectan distancia larga = paso libre
    return (distanceData.frontLeft > DISTANCE_POOL_MAX && 
            distanceData.frontRight > DISTANCE_POOL_MAX);
}

ColorSensorData Sensors::readColorSensor(int sensorNumber) {
    ColorSensorData data;
    
    selectMuxChannel(sensorNumber);
    delay(5);
    
    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
    
    if (!tcs.begin()) {
        data.type = NONE;
        return data;
    }
    
    tcs.getRawData(&data.r, &data.g, &data.b, &data.c);
    data.type = identifyBean(data);
    
    return data;
}

BeanType Sensors::identifyBean(ColorSensorData color) {
    // Identificar tipo de grano por color
    if (color.r >= RIPE_R_MIN && color.r <= RIPE_R_MAX &&
        color.g >= RIPE_G_MIN && color.g <= RIPE_G_MAX &&
        color.b >= RIPE_B_MIN && color.b <= RIPE_B_MAX) {
        return RIPE;
    }
    
    if (color.g >= GREEN_G_MIN && color.g <= GREEN_G_MAX &&
        color.r >= GREEN_R_MIN && color.r <= GREEN_R_MAX) {
        return GREEN;
    }
    
    if (color.b >= OVERRIPE_B_MIN && color.b <= OVERRIPE_B_MAX) {
        return OVERRIPE;
    }
    
    return NONE;
}

IMUData Sensors::getIMUData() {
    return imuData;
}

float Sensors::getHeading() {
    return imuData.heading;
}