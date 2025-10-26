#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <SensorDistancia.h>

// --- Configuración ---
#define TCA_ADDR 0x70  // Dirección I2C del multiplexor TCA9548A
SensorDistancia sensorD; 
// Crea un objeto para cada sensor.
Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();

// Función auxiliar para cambiar el canal del multiplexor
void selectChannel(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  // Espera a que el Monitor Serie se conecte (necesario en Teensy)
  while (!Serial && millis() < 4000) {}

  // Inicia el bus I2C principal del Teensy 4.1 (pines 18 y 19)
  Wire.begin();

  Serial.println("Teensy 4.1, TCA9548A y 2x VL53L0X");

  // --- Inicializar Sensor 1 en el Canal 0 ---
  selectChannel(0);
  Serial.print("Iniciando Sensor 1 en Canal 0... ");
  if (!sensor1.begin()) {
    Serial.println("¡Fallo al iniciar Sensor 1! Revisa conexiones.");
    while (1); // Detiene el programa si falla
  }
  Serial.println("OK!");

  // --- Inicializar Sensor 2 en el Canal 1 ---
  selectChannel(1);
  Serial.print("Iniciando Sensor 2 en Canal 1... ");
  if (!sensor2.begin()) {
    Serial.println("¡Fallo al iniciar Sensor 2! Revisa conexiones.");
    while (1); // Detiene el programa si falla
  }
  Serial.println("OK!");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure1;
  VL53L0X_RangingMeasurementData_t measure2;

  // --- Leer Sensor 1 ---
  selectChannel(0); // ¡Importante! Selecciona el canal antes de leer
  sensor1.rangingTest(&measure1, false);

  // --- Leer Sensor 2 ---
  selectChannel(1); // Cambia al otro canal
  sensor2.rangingTest(&measure2, false);

  // Imprime la lectura del Sensor 1
  Serial.print("Sensor 1: ");
  if (measure1.RangeStatus != 4) { // Status 4 indica un error o fuera de rango
    Serial.print(measure1.RangeMilliMeter);
    Serial.print(" mm");
  } else {
    Serial.print("Fuera de rango");
  }

  Serial.print("  |  ");

  // Imprime la lectura del Sensor 2
  Serial.print("Sensor 2: ");
  if (measure2.RangeStatus != 4) {
     sensorD.distancia(); 
  }

  Serial.println(); // Salto de línea

  delay(100); // Espera 100ms entre cada ciclo de lecturas
}