#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <calibrate_bodemvochtigheidssensoren.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

#define ONE_WIRE_BUS 17

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

float temperature = 0;

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

unsigned long wateringTimeInterval = 0;
boolean WATER_SWITCH;

byte resistanceHumidityCategory = 0;
byte capacitanceHumidityCategory = 0;
byte finalHumidityCategory = 0;
const byte LEGENDA_NO_VALUE = 0;
const byte LEGENDA_DRY = 1;
const byte LEGENDA_WET = 2;
const byte LEGENDA_WATER = 3;

const int TIME_INTERVAL = 5000;
unsigned long timer = 0;

byte get_resistance_category(int sensorValue) {
  if (sensorValue >= RESISTANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_DRY_INTERVAL_MAX) {
    return LEGENDA_DRY;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WET_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WET_INTERVAL_MAX) {
    return LEGENDA_WET;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WATER_INTERVAL_MAX) {
    return LEGENDA_WATER;
  }
  else {
    return LEGENDA_NO_VALUE;
  }
}

byte get_capacitance_category(int sensorValue) {
  if (sensorValue >= CAPACITANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_DRY_INTERVAL_MAX) {
    return LEGENDA_DRY;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WET_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_WET_INTERVAL_MAX) {
    return LEGENDA_WET;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_WATER_INTERVAL_MAX) {
    return LEGENDA_WATER;
  }
  else {
    return LEGENDA_NO_VALUE;
  }
}

byte get_final_category(byte resistanceCategory, byte capacitanceCategory) {
  if (capacitanceCategory == LEGENDA_DRY) {
    return LEGENDA_DRY;
  }
  else if (capacitanceCategory == LEGENDA_WET) {
    if (resistanceCategory == LEGENDA_DRY) {
      return LEGENDA_DRY;
    }
    else {
      return LEGENDA_WET;
    }
  }
  else if (capacitanceCategory == LEGENDA_WATER) {
    if (resistanceCategory == LEGENDA_DRY) {
      return LEGENDA_DRY;
    }
    else if (resistanceCategory == LEGENDA_WET) {
      return LEGENDA_WET;
    }
    else {
      return LEGENDA_WATER;
    }
  }
  else {
    return resistanceCategory;
  }
}

float get_temperature() {
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperature in Celsius
  return sensors.getTempCByIndex(0);
}

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  resistanceHumidityValue = 0;
  capacitanceHumidityValue = 0;

  Serial.begin(9600);
  timer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (millis() - timer >= TIME_INTERVAL) {
    timer = millis();

    resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
    capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);

    resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);
    capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);
    finalHumidityCategory = get_final_category(resistanceHumidityCategory, capacitanceHumidityCategory);

    if (WATER_SWITCH == LOW && finalHumidityCategory >= LEGENDA_WET) {
      temperature = get_temperature();

      if (temperature > 25) {
        wateringTimeInterval = 2000;
        WATER_SWITCH = HIGH;
      }
      else if (temperature > 5) {
        wateringTimeInterval = 1000;
        WATER_SWITCH = HIGH;
      }
    }
  }
}