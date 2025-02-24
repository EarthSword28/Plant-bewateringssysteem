#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <constants.h>

#define ARDUINOTRACE_ENABLE 1  // Disable(0)/Enable(1) all traces
#include <ArduinoTrace.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITANCE_HUMIDITY_SENSOR 39

#define ONE_WIRE_BUS 12                   // temperature sensor

#define RELAY_MODULE 17                   // the relay for the pump
 
//Temperature chip i/o
OneWire ds(ONE_WIRE_BUS);  // on digital pin 12

// Setup a oneWire instance to communicate with any OneWire device
//OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
//DallasTemperature sensors(&oneWire);

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
  TRACE();
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

  // Send the command to get temperatures
  //sensors.requestTemperatures(); 

  //return the temperature in Celsius
  //return sensors.getTempCByIndex(0);
}

int get_resistance_category(int sensorValue) {
  TRACE();
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
  TRACE();
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

int read_sensors_and_give_water_if_neccesary(byte humidityCategory, int temp) {
  TRACE();
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
  TRACE();
  wateringTimer = millis();
  pumpSwitch = HIGH;
  digitalWrite(RELAY_MODULE, HIGH);
}

void stop_watering() {
  TRACE();
  timer = millis();
  pumpSwitch = LOW;
  wateringTimeInterval = WATERING_TIME_INTERVAL_INACTIVE;
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
  //sensors.begin(); 
}

void loop() {
  if (wateringTimeInterval != 0) {
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

    resistanceHumidityValue = analogRead(RESISTANCE_HUMIDITY_SENSOR);
    capacitanceHumidityValue = analogRead(CAPACITANCE_HUMIDITY_SENSOR);
    DUMP(resistanceHumidityValue);
    DUMP(capacitanceHumidityValue);
    BREAK();

    resistanceHumidityCategory = get_resistance_category(resistanceHumidityValue);
    capacitanceHumidityCategory = get_capacitance_category(capacitanceHumidityValue);
    finalHumidityCategory = get_final_category(resistanceHumidityCategory, capacitanceHumidityCategory);
    DUMP(resistanceHumidityCategory);
    DUMP(capacitanceHumidityCategory);
    DUMP(finalHumidityCategory);
    BREAK();

    temperature = get_temperature();
    DUMP(temperature);
    BREAK();

    wateringTimeInterval = read_sensors_and_give_water_if_neccesary(finalHumidityCategory, temperature);
    DUMP(wateringTimeInterval);

    BREAK();
  }
}