#include <Arduino.h>
#include "SensorColor.h"
#include "ColorConverterLib.h"
#include "SensorDistancia.h"

extern SensorColor sensorC;
extern SensorDistancia sensorD;

void setup() {
  Serial.begin(9600);
  delay(1000);

  /*if (!sensorC.begin()) {
    Serial.println("Error: no se pudo inicializar el sensor de color.");
    while (1); 
  }*/
  if (!sensorD.begin()) {
    Serial.println("Error: no se pudo inicializar el sensor de distancia.");
    while (1); 
  }
  //  calibre.
  /*Serial.println("Coloque el sensor sobre una superficie blanca y presione Reset.");
  delay(5000); // Espera 5 segundos para colocar el sensor.
  sensorC.calibrarBlanco();
  Serial.println("Comenzando lecturas...");*/
  
}

void loop() {
 /* sensor.leerColor(); // Lee, normaliza y convierte.
  sensorC.imprimir(); 
  sensorC.definir(); 
  delay(500); */

}