#include <Arduino.h>
#include <OneWire.h>

#include <calibrate_bodemvochtigheidssensoren.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

byte resistanceHumidityCategory = 0;      // Legenda: NO Measurement = 0, DRY = 1, WET = 2, WATER = 3
byte capacitanceHumidityCategory = 0;     // Legenda: NO Measurement = 0, DRY = 1, WET = 2, WATER = 3
byte finalHumidityCategory = 0;           // Legenda: NO Measurement = 0, DRY = 1, WET = 2, WATER = 3

const int TIME_INTERVAL = 2500;
unsigned long timer = 0;

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  resistanceHumidityValue = 0;
  capacitanceHumidityValue = 0;

  Serial.begin(9600);
  timer = millis();
}

void loop() {
  if (millis() - timer >= TIME_INTERVAL) {
    timer = millis();
    resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
    capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);
  }
}