#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Adafruit_TCS34725.h>
#include "../Config/pins.h"
#include "../Config/constants.h"

// ============================================
// CLASE DE SENSORES DE LÍNEA
// ============================================

class LineSensors {
public:
    LineSensors();
    
    void begin();
    void update();
    void calibrate(uint16_t black_threshold[8], uint16_t white_threshold[8]);
    
    // Lecturas individuales
    uint16_t getRaw(uint8_t sensor) const;
    bool isOnLine(uint8_t sensor) const;
    
    // Detección de bordes del campo
    bool detectFrontEdge() const;
    bool detectBackEdge() const;
    bool detectLeftEdge() const;
    bool detectRightEdge() const;
    bool detectAnyEdge() const;
    
    // Para seguidor de línea
    int16_t getLinePosition(); // -100 a +100, 0 = centrado
    bool isLineDetected() const;
    
    void printReadings();
    
private:
    uint16_t readings[8];
    uint16_t black_threshold[8];
    uint16_t white_threshold[8];
    bool on_line[8];
    
    uint8_t pins[8] = {
        LineSensorPins::FRONT_LEFT,
        LineSensorPins::FRONT_RIGHT,
        LineSensorPins::BACK_LEFT,
        LineSensorPins::BACK_RIGHT,
        LineSensorPins::LEFT_FRONT,
        LineSensorPins::LEFT_BACK,
        LineSensorPins::RIGHT_FRONT,
        LineSensorPins::RIGHT_BACK
    };
    
    void updateSensor(uint8_t index);
};

// ============================================
// CLASE DE SENSORES DE DISTANCIA (ToF)
// ============================================

class DistanceSensors {
public:
    DistanceSensors();
    
    bool begin(TwoWire& wire = Wire);
    void update();
    
    // Lecturas individuales
    uint16_t getFrontLeft() const;
    uint16_t getFrontRight() const;
    uint16_t getBackLeft() const;
    uint16_t getBackRight() const;
    uint16_t getArm() const;
    
    // Detección de condiciones
    bool isPathClear() const;
    bool isObstacleNear() const;
    bool detectPool() const;
    bool detectOpenPath() const; // Entre las piscinas
    
    // Estado
    bool isValid(uint8_t sensor) const;
    bool allSensorsReady() const;
    
    void printReadings();
    
private:
    VL53L0X sensors[5];
    uint16_t distances[5];
    bool sensor_ready[5];
    
    const uint8_t mux_channels[5] = {
        I2C::MuxChannel::TOF_FRONT_LEFT,
        I2C::MuxChannel::TOF_FRONT_RIGHT,
        I2C::MuxChannel::TOF_BACK_LEFT,
        I2C::MuxChannel::TOF_BACK_RIGHT,
        I2C::MuxChannel::TOF_ARM
    };
    
    void selectMuxChannel(uint8_t channel);
    bool initSensor(uint8_t index);
};

// ============================================
// CLASE DE SENSORES DE COLOR
// ============================================

struct ColorReading {
    uint16_t r, g, b, c;  // Red, Green, Blue, Clear
    BeanType bean_type;
    uint16_t confidence;   // 0-100
};

class ColorSensors {
public:
    ColorSensors();
    
    bool begin(TwoWire& wire = Wire);
    void update();
    
    // Lecturas
    ColorReading getReading(uint8_t sensor) const;
    BeanType detectBeanType(uint8_t sensor) const;
    bool isBeanPresent(uint8_t sensor) const;
    
    // Calibración
    void calibrate(const CalibrationData& cal);
    void captureColorSample(uint8_t sensor);
    
    void printReadings();
    
private:
    Adafruit_TCS34725 sensors[3];
    ColorReading readings[3];
    bool sensor_ready[3];
    
    const uint8_t mux_channels[3] = {
        I2C::MuxChannel::COLOR_SENSOR_1,
        I2C::MuxChannel::COLOR_SENSOR_2,
        I2C::MuxChannel::COLOR_SENSOR_3
    };
    
    // Valores calibrados
    struct {
        uint16_t r, g, b;
    } calibrated_colors[6]; // Red, Orange, Yellow, Green, Blue, Black
    
    uint16_t color_tolerance;
    
    void selectMuxChannel(uint8_t channel);
    bool initSensor(uint8_t index);
    BeanType classifyColor(uint16_t r, uint16_t g, uint16_t b);
    uint16_t calculateConfidence(uint16_t r, uint16_t g, uint16_t b, BeanType type);
    uint16_t colorDistance(uint16_t r1, uint16_t g1, uint16_t b1, 
                          uint16_t r2, uint16_t g2, uint16_t b2);
};

// ============================================
// IMPLEMENTACIÓN - LINE SENSORS
// ============================================

inline LineSensors::LineSensors() {
    for(int i = 0; i < 8; i++) {
        readings[i] = 0;
        on_line[i] = false;
        black_threshold[i] = Threshold::Line::BLACK_LINE;
        white_threshold[i] = Threshold::Line::WHITE_SURFACE;
    }
}

inline void LineSensors::begin() {
    for(int i = 0; i < 8; i++) {
        pinMode(pins[i], INPUT);
    }
    Serial.println("Line sensors initialized");
}

inline void LineSensors::update() {
    for(int i = 0; i < 8; i++) {
        updateSensor(i);
    }
}

inline void LineSensors::updateSensor(uint8_t index) {
    readings[index] = analogRead(pins[index]);
    
    // Detectar si está sobre línea negra
    if(readings[index] > black_threshold[index]) {
        on_line[index] = true;
    } else if(readings[index] < white_threshold[index]) {
        on_line[index] = false;
    }
    // Hystéresis - mantiene estado si está en zona intermedia
}

inline uint16_t LineSensors::getRaw(uint8_t sensor) const {
    return readings[sensor];
}

inline bool LineSensors::isOnLine(uint8_t sensor) const {
    return on_line[sensor];
}

inline bool LineSensors::detectFrontEdge() const {
    return on_line[0] || on_line[1]; // FRONT_LEFT o FRONT_RIGHT
}

inline bool LineSensors::detectBackEdge() const {
    return on_line[2] || on_line[3]; // BACK_LEFT o BACK_RIGHT
}

inline bool LineSensors::detectLeftEdge() const {
    return on_line[4] || on_line[5]; // LEFT_FRONT o LEFT_BACK
}

inline bool LineSensors::detectRightEdge() const {
    return on_line[6] || on_line[7]; // RIGHT_FRONT o RIGHT_BACK
}

inline bool LineSensors::detectAnyEdge() const {
    return detectFrontEdge() || detectBackEdge() || 
           detectLeftEdge() || detectRightEdge();
}

inline int16_t LineSensors::getLinePosition() {
    // Calcular posición de la línea usando sensores frontales
    // Retorna -100 (izquierda) a +100 (derecha), 0 = centrado
    
    int left_value = readings[0];   // FRONT_LEFT
    int right_value = readings[1];  // FRONT_RIGHT
    
    // Calcular diferencia normalizada
    int diff = right_value - left_value;
    int sum = left_value + right_value;
    
    if(sum == 0) return 0;
    
    return (diff * 100) / sum;
}

inline bool LineSensors::isLineDetected() const {
    return on_line[0] || on_line[1]; // Al menos un sensor frontal detecta línea
}

inline void LineSensors::calibrate(uint16_t black[8], uint16_t white[8]) {
    for(int i = 0; i < 8; i++) {
        black_threshold[i] = black[i];
        white_threshold[i] = white[i];
    }
    Serial.println("Line sensors calibrated");
}

inline void LineSensors::printReadings() {
    Serial.print("Line: ");
    for(int i = 0; i < 8; i++) {
        Serial.printf("%4d%s ", readings[i], on_line[i] ? "*" : " ");
    }
    Serial.println();
}

// ============================================
// IMPLEMENTACIÓN - DISTANCE SENSORS
// ============================================

inline DistanceSensors::DistanceSensors() {
    for(int i = 0; i < 5; i++) {
        distances[i] = 0;
        sensor_ready[i] = false;
    }
}

inline bool DistanceSensors::begin(TwoWire& wire) {
    bool all_ok = true;
    
    for(int i = 0; i < 5; i++) {
        if(!initSensor(i)) {
            Serial.printf("ToF sensor %d failed to initialize\n", i);
            all_ok = false;
        }
    }
    
    if(all_ok) {
        Serial.println("All distance sensors initialized");
    }
    
    return all_ok;
}

inline bool DistanceSensors::initSensor(uint8_t index) {
    selectMuxChannel(mux_channels[index]);
    delay(10);
    
    sensors[index].setTimeout(500);
    if(sensors[index].init()) {
        sensors[index].startContinuous();
        sensor_ready[index] = true;
        return true;
    }
    
    sensor_ready[index] = false;
    return false;
}

inline void DistanceSensors::selectMuxChannel(uint8_t channel) {
    Wire.beginTransmission(I2C::Address::TCA9548A);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

inline void DistanceSensors::update() {
    for(int i = 0; i < 5; i++) {
        if(sensor_ready[i]) {
            selectMuxChannel(mux_channels[i]);
            distances[i] = sensors[i].readRangeContinuousMillimeters();
            
            // Validar lectura
            if(distances[i] >= Threshold::Distance::INVALID_READING) {
                distances[i] = Threshold::Distance::MAX_RANGE_MM;
            }
        }
    }
}

inline uint16_t DistanceSensors::getFrontLeft() const {
    return distances[0];
}

inline uint16_t DistanceSensors::getFrontRight() const {
    return distances[1];
}

inline uint16_t DistanceSensors::getBackLeft() const {
    return distances[2];
}

inline uint16_t DistanceSensors::getBackRight() const {
    return distances[3];
}

inline uint16_t DistanceSensors::getArm() const {
    return distances[4];
}

inline bool DistanceSensors::isPathClear() const {
    return (getFrontLeft() > Threshold::Distance::PATH_CLEAR_MM &&
            getFrontRight() > Threshold::Distance::PATH_CLEAR_MM);
}

inline bool DistanceSensors::isObstacleNear() const {
    return (getFrontLeft() < Threshold::Distance::OBSTACLE_NEAR_MM ||
            getFrontRight() < Threshold::Distance::OBSTACLE_NEAR_MM);
}

inline bool DistanceSensors::detectPool() const {
    uint16_t fl = getFrontLeft();
    uint16_t fr = getFrontRight();
    
    // Una piscina bloquea el camino pero no ambos sensores
    return (fl < Threshold::Distance::POOL_DETECTION_MM ||
            fr < Threshold::Distance::POOL_DETECTION_MM) &&
           !(fl < Threshold::Distance::POOL_DETECTION_MM &&
             fr < Threshold::Distance::POOL_DETECTION_MM);
}

inline bool DistanceSensors::detectOpenPath() const {
    // Ambos sensores ven largo alcance = camino despejado entre piscinas
    return (getFrontLeft() > Threshold::Distance::PATH_CLEAR_MM &&
            getFrontRight() > Threshold::Distance::PATH_CLEAR_MM);
}

inline bool DistanceSensors::isValid(uint8_t sensor) const {
    return sensor_ready[sensor] && 
           distances[sensor] < Threshold::Distance::INVALID_READING;
}

inline bool DistanceSensors::allSensorsReady() const {
    for(int i = 0; i < 5; i++) {
        if(!sensor_ready[i]) return false;
    }
    return true;
}

inline void DistanceSensors::printReadings() {
    Serial.printf("ToF: FL=%4d FR=%4d BL=%4d BR=%4d ARM=%4d mm\n",
        distances[0], distances[1], distances[2], distances[3], distances[4]);
}

#endif // SENSORS_H