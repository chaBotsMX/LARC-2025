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
    
    Serial.println("Inicializando sensores ToF con multiplexor...");
    
    // --- Inicializar Sensor ToF Front Left en Canal 4 ---
    selectMuxChannel(MUX_TOF_FL);
    Serial.print("  Sensor ToF Front Left (Canal ");
    Serial.print(MUX_TOF_FL);
    Serial.print(")... ");
    if (!tofFL.begin()) {
        Serial.println("ERROR!");
    } else {
        Serial.println("OK!");
    }
    delay(50);
    
    // --- Inicializar Sensor ToF Front Right en Canal 5 ---
    selectMuxChannel(MUX_TOF_FR);
    Serial.print("  Sensor ToF Front Right (Canal ");
    Serial.print(MUX_TOF_FR);
    Serial.print(")... ");
    if (!tofFR.begin()) {
        Serial.println("ERROR!");
    } else {
        Serial.println("OK!");
    }
    delay(50);
    
    // --- Inicializar Sensor ToF Rear Left en Canal 6 ---
    selectMuxChannel(MUX_TOF_RL);
    Serial.print("  Sensor ToF Rear Left (Canal ");
    Serial.print(MUX_TOF_RL);
    Serial.print(")... ");
    if (!tofRL.begin()) {
        Serial.println("ERROR!");
    } else {
        Serial.println("OK!");
    }
    delay(50);
    
    // --- Inicializar Sensor ToF Rear Right en Canal 7 ---
    selectMuxChannel(MUX_TOF_RR);
    Serial.print("  Sensor ToF Rear Right (Canal ");
    Serial.print(MUX_TOF_RR);
    Serial.print(")... ");
    if (!tofRR.begin()) {
        Serial.println("ERROR!");
    } else {
        Serial.println("OK!");
    }
    delay(50);
    
    // Inicializar IMU
    Serial.print("Inicializando IMU BNO055... ");
    if (!bno.begin()) {
        Serial.println("ERROR! No se pudo inicializar BNO055");
    } else {
        Serial.println("OK!");
        bno.setExtCrystalUse(true);
    }
    
    delay(100);
}

void Sensors::selectMuxChannel(uint8_t channel) {
    if (channel > 7) return;
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
    
    /* 
    // Actualizar sensores de distancia ToF
    distanceData.frontLeft = readToF(MUX_TOF_FL, tofFL);
    distanceData.frontRight = readToF(MUX_TOF_FR, tofFR);
    distanceData.rearLeft = readToF(MUX_TOF_RL, tofRL);
    distanceData.rearRight = readToF(MUX_TOF_RR, tofRR);
    
    // Actualizar IMU
    sensors::event_t event;
    bno.getEvent(&event);
    imuData.heading = event.orientation.x;
    imuData.pitch = event.orientation.y;
    imuData.roll = event.orientation.z;
    */
}
 

bool Sensors::readLineSensor(int pin) {
    int value = digitalRead(pin);
    return value ; // true si detecta línea negra
}

int Sensors::readToF(uint8_t channel, Adafruit_VL53L0X &sensor) {
    selectMuxChannel(channel);
    delay(5); // Pequeño delay para estabilizar el multiplexor
    
    VL53L0X_RangingMeasurementData_t measure;
    sensor.rangingTest(&measure, false);
    
    if (measure.RangeStatus != 4) { // Status 4 = fuera de rango
        return measure.RangeMilliMeter;
    }
    return -1; // Error o fuera de rango
}

LineSensorData Sensors::getLineSensors() {
    return lineData;
}

DistanceSensorData Sensors::getDistanceSensors() {
    return distanceData;
}

bool Sensors::detectObstacleAhead() {
    return (distanceData.frontLeft < DISTANCE_OBSTACLE_STOP && distanceData.frontLeft > 0) ||
           (distanceData.frontRight < DISTANCE_OBSTACLE_STOP && distanceData.frontRight > 0);
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