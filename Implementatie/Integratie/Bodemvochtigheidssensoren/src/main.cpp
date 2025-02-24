#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <constants.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

#define ONE_WIRE_BUS 12                   // temperature sensor

#define RELAY_MODULE 17                   // the relay for the pump
 
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

float temperature = 0;

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

unsigned long wateringTimeInterval = 0;
boolean pumpSwitch;

unsigned short resistanceHumidityCategory = 0;
unsigned short capacitanceHumidityCategory = 0;
unsigned short finalHumidityCategory = 0;

unsigned long timer = 0;
unsigned long wateringTimer = 0;

float get_temperature() {
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperature in Celsius
  return sensors.getTempCByIndex(0);
}

int get_resistance_category(int sensorValue) {
  if (sensorValue >= RESISTANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_DRY_INTERVAL_MAX) {
    return HUMIDITY_DRY;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WET_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WET_INTERVAL_MAX) {
    return HUMIDITY_WET;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WATER_INTERVAL_MAX) {
    return HUMIDITY_WATER;
  }
  else {
    return HUMIDITY_NO_VALUE;
  }
}

int get_capacitance_category(int sensorValue) {
  if (sensorValue >= CAPACITANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_DRY_INTERVAL_MAX) {
    return HUMIDITY_DRY;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WET_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_WET_INTERVAL_MAX) {
    return HUMIDITY_WET;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_WATER_INTERVAL_MAX) {
    return HUMIDITY_WATER;
  }
  else {
    return HUMIDITY_NO_VALUE;
  }
}

int get_final_category(int resistanceCategory, int capacitanceCategory) {
  if (capacitanceCategory == HUMIDITY_DRY) {
    return HUMIDITY_DRY;
  }
  else if (capacitanceCategory == HUMIDITY_WET) {
    if (resistanceCategory == HUMIDITY_DRY) {
      return HUMIDITY_DRY;
    }
    else {
      return HUMIDITY_WET;
    }
  }
  else if (capacitanceCategory == HUMIDITY_WATER) {
    if (resistanceCategory == HUMIDITY_DRY) {
      return HUMIDITY_DRY;
    }
    else if (resistanceCategory == HUMIDITY_WET) {
      return HUMIDITY_WET;
    }
    else {
      return HUMIDITY_WATER;
    }
  }
  else {
    return resistanceCategory;
  }
}

int read_sensors_and_give_water_if_neccesary(byte humidityCategory, int temp) {
  if (humidityCategory == HUMIDITY_DRY) {
    if (temp > MAX_TEMPERATURE) {
      return WATERING_TIME_INTERVAL_LONG;
    }
    else if (temp > MIN_TEMPERATURE) {
      return WATERING_TIME_INTERVAL_SHORT;
    }
    else {
      return WATERING_TIME_INTERVAL_INACTIVE;
    }
  }
  else {
    return WATERING_TIME_INTERVAL_INACTIVE;
  }
}

void start_watering() {
  wateringTimer = millis();
  pumpSwitch = HIGH;
  digitalWrite(RELAY_MODULE, HIGH);
}

void stop_watering() {
  timer = millis();
  pumpSwitch = LOW;
  digitalWrite(RELAY_MODULE, LOW);
}

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(RELAY_MODULE, OUTPUT);
  resistanceHumidityValue = 0;
  capacitanceHumidityValue = 0;
  digitalWrite(RELAY_MODULE, LOW);

  pumpSwitch = LOW;
  wateringTimeInterval = 0;

  Serial.begin(9600);
  timer = millis();
  wateringTimer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (wateringTimeInterval != 0) {
    if (pumpSwitch == LOW) {
      start_watering();
    }
    else if (millis() - wateringTimer >= wateringTimeInterval) {
      stop_watering();
    }
  }
  else if (millis() - timer >= TIME_INTERVAL_SENSORS) {
    timer = millis();

    resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
    capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);

    resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);
    capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);
    finalHumidityCategory = get_final_category(resistanceHumidityCategory, capacitanceHumidityCategory);

    temperature = get_temperature();

    wateringTimeInterval = read_sensors_and_give_water_if_neccesary(finalHumidityCategory, temperature);
  }
}