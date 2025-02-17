#include <Arduino.h>
#include <OneWire.h>

#include <calibrate_bodemvochtigheidssensoren.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

byte resistanceHumidityCategory = 0;
byte capacitanceHumidityCategory = 0;
byte finalHumidityCategory = 0;
const byte LEGENDA_NO_VALUE = 0;
const byte LEGENDA_DRY = 1;
const byte LEGENDA_WET = 2;
const byte LEGENDA_WATER = 3;

const int TIME_INTERVAL = 2500;
unsigned long timer = 0;

byte get_resistance_category(int sensorValue) {
  if (sensorValue >= RESISTANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_DRY_INTERVAL_MAX) {
    return LEGENDA_DRY;
  }
  if (sensorValue >= RESISTANCE_SENSOR_WET_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WET_INTERVAL_MAX) {
    return LEGENDA_WET;
  }
  if (sensorValue >= RESISTANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WATER_INTERVAL_MAX) {
    return LEGENDA_WATER;
  }
}

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