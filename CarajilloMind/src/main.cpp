#include <Arduino.h>
#include "SensorColor.h"

// put function declarations here:
int myFunction(int, int);

SensorColor sensor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  if (!sensor.begin()) {
    Serial.println("Error: no se pudo inicializar el sensor de color.");
    while (1); // Detener si no se detecta
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor.leerColor();
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}