#include "debug.h"

Debug debug;

void Debug::init() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000); // Esperar serial con timeout
    Serial.println("Sistema de debug inicializado");
}

void Debug::printDivider() {
    Serial.println("----------------------------------------");
}

void Debug::printLineSensors() {
    LineSensorData line = sensors.getLineSensors();
    
    Serial.println("SENSORES DE LÍNEA:");
    Serial.print("  Front Left: ");
    Serial.println(line.frontLeft ? "DETECTA" : "NO");
    Serial.print("  Front Right: ");
    Serial.println(line.frontRight ? "DETECTA" : "NO");
    Serial.print("  Rear Left: ");
    Serial.println(line.rearLeft ? "DETECTA" : "NO");
    Serial.print("  Rear Right: ");
    Serial.println(line.rearRight ? "DETECTA" : "NO");
}

void Debug::printDistanceSensors() {
    DistanceSensorData dist = sensors.getDistanceSensors();
    
    Serial.println("SENSORES DE DISTANCIA (mm):");
    Serial.print("  Front Left: ");
    Serial.println(dist.frontLeft);
    Serial.print("  Front Right: ");
    Serial.println(dist.frontRight);
    Serial.print("  Rear Left: ");
    Serial.println(dist.rearLeft);
    Serial.print("  Rear Right: ");
    Serial.println(dist.rearRight);
}

void Debug::printIMU() {
    IMUData imu = sensors.getIMUData();
    
    Serial.println("IMU:");
    Serial.print("  Heading: ");
    Serial.print(imu.heading);
    Serial.println("°");
    Serial.print("  Pitch: ");
    Serial.print(imu.pitch);
    Serial.println("°");
    Serial.print("  Roll: ");
    Serial.print(imu.roll);
    Serial.println("°");
}

void Debug::printColorSensor(int sensorNum) {
    ColorSensorData color = sensors.readColorSensor(sensorNum);
    
    Serial.print("SENSOR DE COLOR ");
    Serial.print(sensorNum);
    Serial.println(":");
    Serial.print("  R: ");
    Serial.print(color.r);
    Serial.print(" G: ");
    Serial.print(color.g);
    Serial.print(" B: ");
    Serial.print(color.b);
    Serial.print(" C: ");
    Serial.println(color.c);
    Serial.print("  Tipo: ");
    switch (color.type) {
        case RIPE: Serial.println("MADURO"); break;
        case GREEN: Serial.println("VERDE"); break;
        case OVERRIPE: Serial.println("SOBREMADURO"); break;
        default: Serial.println("NINGUNO"); break;
    }
}

void Debug::printSensorData() {
    printDivider();
    Serial.println("ESTADO DE SENSORES");
    printDivider();
    
    printLineSensors();
    Serial.println();
    
    printDistanceSensors();
    Serial.println();
    
    printIMU();
    
    printDivider();
}

void Debug::printSystemStatus() {
    printDivider();
    Serial.println("ESTADO DEL SISTEMA");
    printDivider();
    Serial.print("Tiempo de ejecución: ");
    Serial.print(millis() / 1000);
    Serial.println(" segundos");
    
    printDivider();
}