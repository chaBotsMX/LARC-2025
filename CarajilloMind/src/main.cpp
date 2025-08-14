#include <Arduino.h>
#include "SensorColor.h"

// put function declarations here:
int myFunction(int, int);

SensorColor sensor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  if (!sensor.begin()) {
    Serial.println("Error: no se pudo inicializar el sensor de color.");
    while (1); // Detener si no se detecta
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  sensor.leerColor();

  Serial.print("R: "); Serial.print(sensor.getR());
  Serial.print("  G: "); Serial.print(sensor.getG());
  Serial.print("  B: "); Serial.print(sensor.getB());
  Serial.print("  C: "); Serial.println(sensor.getC());

  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}