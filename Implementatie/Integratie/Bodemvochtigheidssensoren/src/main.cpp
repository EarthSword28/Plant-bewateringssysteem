#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>

#define ARDUINOTRACE_ENABLE TRACE_SWITCH  // Enable(1)/Disable(0) all traces
#include <ArduinoTrace.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

#define ONE_WIRE_BUS 12                   // temperature sensor

#define PANIC_BUTTON 4

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
boolean panicButtonSwitch;

String waterStatus;

String resistanceHumidityCategory = "";
String capacitanceHumidityCategory = "";
String finalHumidityCategory = "";

unsigned long timer = 0;
unsigned long wateringTimer = 0;
unsigned long panicButtonDebounceTimer = 0;

float get_temperature() {
  TRACE();
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperature in Celsius
  return sensors.getTempCByIndex(0);
}

String get_resistance_category(int sensorValue) {
  TRACE();
  if (sensorValue >= RESISTANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue < RESISTANCE_SENSOR_DRY_INTERVAL_MAX) {
    return HUMIDITY_DRY;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WET_INTERVAL_MIN && sensorValue < RESISTANCE_SENSOR_WET_INTERVAL_MAX) {
    return HUMIDITY_WET;
  }
  else if (sensorValue >= RESISTANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= RESISTANCE_SENSOR_WATER_INTERVAL_MAX) {
    return HUMIDITY_WATER;
  }
  else {
    return HUMIDITY_NO_VALUE;
  }
}

String get_capacitance_category(int sensorValue) {
  TRACE();
  if (sensorValue >= CAPACITANCE_SENSOR_DRY_INTERVAL_MIN && sensorValue < CAPACITANCE_SENSOR_DRY_INTERVAL_MAX) {
    return HUMIDITY_DRY;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WET_INTERVAL_MIN && sensorValue < CAPACITANCE_SENSOR_WET_INTERVAL_MAX) {
    return HUMIDITY_WET;
  }
  else if (sensorValue >= CAPACITANCE_SENSOR_WATER_INTERVAL_MIN && sensorValue <= CAPACITANCE_SENSOR_WATER_INTERVAL_MAX) {
    return HUMIDITY_WATER;
  }
  else {
    return HUMIDITY_NO_VALUE;
  }
}

String get_final_category(String resistanceCategory, String capacitanceCategory) {
  TRACE();
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

int read_sensors_and_give_water_if_neccesary(String humidityCategory, float temp) {
  TRACE();
  if (humidityCategory == HUMIDITY_DRY) {
    if (temp > MAX_TEMPERATURE) {
      waterStatus = WATER_GEVEN;
      return WATERING_TIME_INTERVAL_LONG;
    }
    else if (temp > MIN_TEMPERATURE) {
      waterStatus = WATER_GEVEN;
      return WATERING_TIME_INTERVAL_SHORT;
    }
    else {
      waterStatus = GEEN_WATER_GEVEN;
      return WATERING_TIME_INTERVAL_INACTIVE;
    }
  }
  else {
    waterStatus = GEEN_WATER_GEVEN;
    return WATERING_TIME_INTERVAL_INACTIVE;
  }
}

void start_watering() {
  TRACE();
  wateringTimer = millis();
  pumpSwitch = HIGH;
  digitalWrite(RELAY_MODULE, HIGH);
  DUMP(wateringTimer);
}

void stop_watering() {
  TRACE();
  timer = millis();
  wateringTimer = millis();
  waterStatus = GEEN_WATER_GEVEN;
  pumpSwitch = LOW;
  wateringTimeInterval = WATERING_TIME_INTERVAL_INACTIVE;
  digitalWrite(RELAY_MODULE, LOW);
  DUMP(wateringTimer);
}

void panic_button() {
  panicButtonSwitch = HIGH;
  panicButtonDebounceTimer = millis();
  waterStatus = HIGH;
  wateringTimeInterval = WATERING_TIME_INTERVAL_PANIC_BUTTON;
}

int get_MOCK_value(boolean randomSwitch, int value, int randomValue1, int randomValue2) {
  if (randomSwitch == LOW) {
    return value;
  }
  else {
    return random(randomValue1, randomValue2);
  }
}

void setup() {
  pinMode(RESISTANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(CAPACITANCE_HUMIDITY_SENSOR, INPUT);
  pinMode(ONE_WIRE_BUS, INPUT);
  pinMode(PANIC_BUTTON, INPUT);
  pinMode(RELAY_MODULE, OUTPUT);
  resistanceHumidityValue = 0;
  capacitanceHumidityValue = 0;
  digitalWrite(RELAY_MODULE, LOW);

  waterStatus = GEEN_WATER_GEVEN;
  pumpSwitch = LOW;
  panicButtonSwitch = LOW;
  wateringTimeInterval = 0;

  Serial.begin(9600);
  timer = millis();
  wateringTimer = millis();
  panicButtonDebounceTimer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (pumpSwitch == LOW && panicButtonSwitch == LOW && digitalRead(PANIC_BUTTON) == HIGH) {
    panic_button();
  }
  else if (panicButtonSwitch == HIGH && millis() - panicButtonDebounceTimer >= PANIC_BUTTON_DEBOUNCE) {
    panicButtonSwitch = LOW;
  }
  if (waterStatus == WATER_GEVEN) {
    TRACE();
    DUMP(pumpSwitch);
    DUMP(RELAY_MODULE);
    if (pumpSwitch == LOW) {
      start_watering();
    }
    else if (millis() - wateringTimer >= wateringTimeInterval) {
      stop_watering();
    }
    DUMP(pumpSwitch);
    DUMP(RELAY_MODULE);
    BREAK();
  }
  else if (millis() - timer >= TIME_INTERVAL_SENSORS) {
    TRACE();
    timer = millis();
    DUMP(timer);
    BREAK();

    if (MOCK_SWITCH == LOW) {
      resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
      capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);
    }
    else {
      resistanceHumidityValue = get_MOCK_value(RESISTANCEE_SENSOR_MOCK_RANDOM, RESISTANCE_SENSOR_MOCK_VALUE, RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_1, RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_2);
      capacitanceHumidityValue = get_MOCK_value(CAPACITANCE_SENSOR_MOCK_RANDOM, CAPACITANCE_SENSOR_MOCK_VALUE, CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_1, CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_2);
    }
    DUMP(resistanceHumidityValue);
    DUMP(capacitanceHumidityValue);
    BREAK();

    resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);                        // resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);
    capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);                     // capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);
    finalHumidityCategory = get_final_category(resistanceHumidityCategory, capacitanceHumidityCategory);
    DUMP(resistanceHumidityCategory);
    DUMP(capacitanceHumidityCategory);
    DUMP(finalHumidityCategory);
    BREAK();

    if (MOCK_SWITCH == LOW) {
      temperature = get_temperature();               // temperature = get_temperature();
    }
    else {
      temperature = get_MOCK_value(TEMPERATURE_SENSOR_MOCK_RANDOM, TEMPERATURE_SENSOR_MOCK_VALUE, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_1, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_2);
    }
    DUMP(temperature);
    BREAK();

    wateringTimeInterval = read_sensors_and_give_water_if_neccesary(finalHumidityCategory, temperature);
    DUMP(wateringTimeInterval);

    BREAK();
  }
}