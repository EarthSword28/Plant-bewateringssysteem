#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <constants.h>

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

float temperature = 0;

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

unsigned long wateringTimeInterval = 0;
boolean waterSwitch;

byte resistanceHumidityCategory = 0;
byte capacitanceHumidityCategory = 0;
byte finalHumidityCategory = 0;

unsigned long timer = 0;
unsigned long wateringTimer = 0;

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

void start_watering() {
  digitalWrite(RELAY_MODULE, HIGH);
}

void stop_watering() {
  digitalWrite(RELAY_MODULE, LOW);
}

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(RELAY_MODULE, OUTPUT);
  resistanceHumidityValue = 0;
  capacitanceHumidityValue = 0;
  digitalWrite(RELAY_MODULE, LOW);

  waterSwitch = LOW;

  Serial.begin(9600);
  timer = millis();
  wateringTimer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (waterSwitch == HIGH && millis() - wateringTimer >= wateringTimeInterval) {
    waterSwitch = LOW;
    stop_watering();
  }
  else if (millis() - timer >= TIME_INTERVAL_SENSORS) {
    timer = millis();

    resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
    capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);

    resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);
    capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);
    finalHumidityCategory = get_final_category(resistanceHumidityCategory, capacitanceHumidityCategory);

    if (waterSwitch == LOW && finalHumidityCategory == LEGENDA_DRY) {
      temperature = get_temperature();

      if (temperature > MAX_TEMPERATURE) {
        wateringTimeInterval = WATERING_TIME_INTERVAL_LONG;
        wateringTimer = millis();
        waterSwitch = HIGH;
        start_watering();
      }
      else if (temperature > MIN_TEMPERATURE) {
        wateringTimeInterval = WATERING_TIME_INTERVAL_SHORT;
        wateringTimer = millis();
        waterSwitch = HIGH;
        start_watering();
      }
    }
  }
}