#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>

#define ARDUINOTRACE_ENABLE 1  // Enable(1)/Disable(0) all traces
#include <ArduinoTrace.h>

#define RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR 36
#define CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR 39

#define ONE_WIRE_BUS 12                   // temperatuur sensor

#define PANIC_BUTTON 4

#define RELAY_MODULE 17                   // de relay voor de pomp
 
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

float temperatuur = 0;

unsigned long resistieveBodemvochtigheidsWaarde = 0;
unsigned long capacitieveBodemvochtigheidsWaarde = 0;

unsigned long waterGevenTijdsInterval = 0;

boolean pompSchakelaar;
boolean panicButtonSwitch;

String waterStatus;

String resistieveBodemvochtigheidsCategorie = "";
String capacitieveBodemvochtigheidsCategorie = "";
String finaleBodemvochtigheidsCategorie = "";

unsigned long timer = 0;
unsigned long waterTimer = 0;
unsigned long panicButtonDebounceTimer = 0;

float get_temperature() {
  TRACE();
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperatuur in Celsius
  return sensors.getTempCByIndex(0);
}

String get_resistance_category(int sensorWaarde) {
  TRACE();
  if (sensorWaarde >= RESISTIEVE_SENSOR_DROOG_INTERVAL_MIN && sensorWaarde < RESISTIEVE_SENSOR_DROOG_INTERVAL_MAX) {
    return VOCHTIGHEID_DROOG;
  }
  else if (sensorWaarde >= RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MIN && sensorWaarde < RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MAX) {
    return VOCHTIGHEID_VOCHTIG;
  }
  else if (sensorWaarde >= RESISTIEVE_SENSOR_NAT_INTERVAL_MIN && sensorWaarde <= RESISTIEVE_SENSOR_NAT_INTERVAL_MAX) {
    return VOCHTIGHEID_NAT;
  }
  else {
    return VOCHTIGHEID_GEEN_METING;
  }
}

String get_capacitance_category(int sensorWaarde) {
  TRACE();
  if (sensorWaarde >= CAPACITIEVE_SENSOR_DROOG_INTERVAL_MIN && sensorWaarde < CAPACITIEVE_SENSOR_DROOG_INTERVAL_MAX) {
    return VOCHTIGHEID_DROOG;
  }
  else if (sensorWaarde >= CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MIN && sensorWaarde < CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MAX) {
    return VOCHTIGHEID_VOCHTIG;
  }
  else if (sensorWaarde >= CAPACITIEVE_SENSOR_NAT_INTERVAL_MIN && sensorWaarde <= CAPACITIEVE_SENSOR_NAT_INTERVAL_MAX) {
    return VOCHTIGHEID_NAT;
  }
  else {
    return VOCHTIGHEID_GEEN_METING;
  }
}

String get_final_category(String resistieveCategorie, String capacitieveCategorie) {
  TRACE();
  if (capacitieveCategorie == VOCHTIGHEID_DROOG) {
    return VOCHTIGHEID_DROOG;
  }
  else if (capacitieveCategorie == VOCHTIGHEID_VOCHTIG) {
    if (resistieveCategorie == VOCHTIGHEID_DROOG) {
      return VOCHTIGHEID_DROOG;
    }
    else {
      return VOCHTIGHEID_VOCHTIG;
    }
  }
  else if (capacitieveCategorie == VOCHTIGHEID_NAT) {
    if (resistieveCategorie == VOCHTIGHEID_DROOG) {
      return VOCHTIGHEID_DROOG;
    }
    else if (resistieveCategorie == VOCHTIGHEID_VOCHTIG) {
      return VOCHTIGHEID_VOCHTIG;
    }
    else {
      return VOCHTIGHEID_NAT;
    }
  }
  else {
    return resistieveCategorie;
  }
}

int read_sensors_and_give_water_if_neccesary(String humidityCategory, float temp) {
  TRACE();
  if (humidityCategory == VOCHTIGHEID_DROOG) {
    if (temp > MAX_TEMPERATUUR) {
      waterStatus = WATER_GEVEN;
      return WATER_GEVEN_INTERVAL_LANG;
    }
    else if (temp > MIN_TEMPERATUUR) {
      waterStatus = WATER_GEVEN;
      return WATER_GEVEN_INTERVAL_KORT;
    }
    else {
      waterStatus = GEEN_WATER_GEVEN;
      return WATER_GEVEN_INTERVAL_INACTIEF;
    }
  }
  else {
    waterStatus = GEEN_WATER_GEVEN;
    return WATER_GEVEN_INTERVAL_INACTIEF;
  }
}

void start_watering() {
  TRACE();
  waterTimer = millis();
  pompSchakelaar = HIGH;
  digitalWrite(RELAY_MODULE, HIGH);
  DUMP(waterTimer);
}

void stop_watering() {
  TRACE();
  timer = millis();
  waterTimer = millis();
  waterStatus = GEEN_WATER_GEVEN;
  pompSchakelaar = LOW;
  waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_INACTIEF;
  digitalWrite(RELAY_MODULE, LOW);
  DUMP(waterTimer);
}

void panic_button() {
  panicButtonSwitch = HIGH;
  panicButtonDebounceTimer = millis();
  waterStatus = HIGH;
  waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_PANIC_BUTTON;
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
  pinMode(RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR, INPUT);
  pinMode(CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR, INPUT);
  pinMode(ONE_WIRE_BUS, INPUT);
  pinMode(PANIC_BUTTON, INPUT);
  pinMode(RELAY_MODULE, OUTPUT);
  resistieveBodemvochtigheidsWaarde = 0;
  capacitieveBodemvochtigheidsWaarde = 0;
  digitalWrite(RELAY_MODULE, LOW);

  waterStatus = GEEN_WATER_GEVEN;
  pompSchakelaar = LOW;
  panicButtonSwitch = LOW;
  waterGevenTijdsInterval = 0;

  Serial.begin(9600);
  timer = millis();
  waterTimer = millis();
  panicButtonDebounceTimer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (pompSchakelaar == LOW && panicButtonSwitch == LOW && digitalRead(PANIC_BUTTON) == HIGH) {
    panic_button();
  }
  else if (panicButtonSwitch == HIGH && millis() - panicButtonDebounceTimer >= PANIC_BUTTON_DEBOUNCE) {
    panicButtonSwitch = LOW;
  }
  if (waterStatus == WATER_GEVEN) {
    TRACE();
    DUMP(pompSchakelaar);
    DUMP(RELAY_MODULE);
    if (pompSchakelaar == LOW) {
      start_watering();
    }
    else if (millis() - waterTimer >= waterGevenTijdsInterval) {
      stop_watering();
    }
    DUMP(pompSchakelaar);
    DUMP(RELAY_MODULE);
    BREAK();
  }
  else if (millis() - timer >= TIJD_INTERVAL_SENSOREN) {
    TRACE();
    timer = millis();
    DUMP(timer);
    BREAK();

    if (MOCK_SWITCH == LOW) {
      resistieveBodemvochtigheidsWaarde = analogRead(RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR);
      capacitieveBodemvochtigheidsWaarde = analogRead(CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR);
    }
    else {
      resistieveBodemvochtigheidsWaarde = get_MOCK_value(RESISTANCEE_SENSOR_MOCK_RANDOM, RESISTANCE_SENSOR_MOCK_VALUE, RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_1, RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_2);
      capacitieveBodemvochtigheidsWaarde = get_MOCK_value(CAPACITANCE_SENSOR_MOCK_RANDOM, CAPACITANCE_SENSOR_MOCK_VALUE, CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_1, CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_2);
    }
    DUMP(resistieveBodemvochtigheidsWaarde);
    DUMP(capacitieveBodemvochtigheidsWaarde);
    BREAK();

    resistieveBodemvochtigheidsCategorie = get_resistance_category(resistieveBodemvochtigheidsWaarde);                        // resistieveBodemvochtigheidsCategorie = get_resistance_category(resistieveBodemvochtigheidsWaarde);
    capacitieveBodemvochtigheidsCategorie = get_capacitance_category(capacitieveBodemvochtigheidsWaarde);                     // capacitieveBodemvochtigheidsCategorie = get_capacitance_category(capacitieveBodemvochtigheidsWaarde);
    finaleBodemvochtigheidsCategorie = get_final_category(resistieveBodemvochtigheidsCategorie, capacitieveBodemvochtigheidsCategorie);
    DUMP(resistieveBodemvochtigheidsCategorie);
    DUMP(capacitieveBodemvochtigheidsCategorie);
    DUMP(finaleBodemvochtigheidsCategorie);
    BREAK();

    if (MOCK_SWITCH == LOW) {
      temperatuur = get_temperature();               // temperatuur = get_temperature();
    }
    else {
      temperatuur = get_MOCK_value(TEMPERATURE_SENSOR_MOCK_RANDOM, TEMPERATURE_SENSOR_MOCK_VALUE, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_1, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_2);
    }
    DUMP(temperatuur);
    BREAK();

    waterGevenTijdsInterval = read_sensors_and_give_water_if_neccesary(finaleBodemvochtigheidsCategorie, temperatuur);
    DUMP(waterGevenTijdsInterval);

    BREAK();
  }
}