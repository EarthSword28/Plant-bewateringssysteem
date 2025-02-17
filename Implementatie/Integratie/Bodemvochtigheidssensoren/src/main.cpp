#include <Arduino.h>
#include <OneWire.h>

#include <calibrate_bodemvochtigheidssensoren.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

const int TIME_INTERVAL = 2500;
unsigned long timer = 0;

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  Serial.begin(9600);
  timer = millis();
}

void loop() {
  if (millis() - timer >= TIME_INTERVAL) {
    timer = millis();
  }
}