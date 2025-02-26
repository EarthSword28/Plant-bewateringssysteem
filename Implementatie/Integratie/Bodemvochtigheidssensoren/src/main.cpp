#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>

#define ARDUINOTRACE_ENABLE 1  // Enable(1)/Disable(0) all traces
#include <ArduinoTrace.h>

#define RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR 36
#define CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR 39

#define ONE_WIRE_BUS 12                   // temperatuur sensor

#define PANIC_BUTTON 27                   // gebruik de ingebouwde knop als de panic button

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
boolean panicButtonSchakelaar;

String waterStatus;

String resistieveBodemvochtigheidsCategorie = "";
String capacitieveBodemvochtigheidsCategorie = "";
String finaleBodemvochtigheidsCategorie = "";

unsigned long timer = 0;
unsigned long waterTimer = 0;
unsigned long panicButtonDebounceTimer = 0;

float lees_temperatuur() {
  TRACE();
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperatuur in Celsius
  return sensors.getTempCByIndex(0);
}

String lees_resistieve_bodemvochtigheids_categorie(int sensorWaarde) {
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

String lees_capacitieve_bodemvochtigheids_categorie(int sensorWaarde) {
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

String bereken_finale_bodemvochtigheids_categorie(String resistieveCategorie, String capacitieveCategorie) {
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

int lees_sensoren_en_geef_water_indien_nodig(String categorie, float temp) {
  TRACE();
  if (categorie == VOCHTIGHEID_DROOG) {
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

void zet_waterpomp_aan() {
  TRACE();
  waterTimer = millis();
  pompSchakelaar = HIGH;
  digitalWrite(RELAY_MODULE, HIGH);
  DUMP(waterTimer);
}

void zet_waterpomp_uit() {
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
  panicButtonSchakelaar = HIGH;
  panicButtonDebounceTimer = millis();
  waterStatus = HIGH;
  waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_PANIC_BUTTON;
}

int get_MOCK_value(boolean randomSchakelaar, int waarde, int randomWaarde1, int randomWaarde2) {
  if (randomSchakelaar == LOW) {
    return waarde;
  }
  else {
    return random(randomWaarde1, randomWaarde2);
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
  panicButtonSchakelaar = LOW;
  waterGevenTijdsInterval = 0;

  Serial.begin(9600);
  timer = millis();
  waterTimer = millis();
  panicButtonDebounceTimer = millis();
  
  // Start up the sensor library
  sensors.begin(); 
}

void loop() {
  if (pompSchakelaar == LOW && panicButtonSchakelaar == LOW && digitalRead(PANIC_BUTTON) == HIGH) {
    panic_button();
  }
  else if (panicButtonSchakelaar == HIGH && millis() - panicButtonDebounceTimer >= PANIC_BUTTON_DEBOUNCE) {
    panicButtonSchakelaar = LOW;
  }
  if (waterStatus == WATER_GEVEN) {
    TRACE();
    DUMP(pompSchakelaar);
    DUMP(RELAY_MODULE);
    if (pompSchakelaar == LOW) {
      zet_waterpomp_aan();
    }
    else if (millis() - waterTimer >= waterGevenTijdsInterval) {
      zet_waterpomp_uit();
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

    resistieveBodemvochtigheidsCategorie = lees_resistieve_bodemvochtigheids_categorie(resistieveBodemvochtigheidsWaarde);                        // resistieveBodemvochtigheidsCategorie = lees_resistieve_bodemvochtigheids_categorie(resistieveBodemvochtigheidsWaarde);
    capacitieveBodemvochtigheidsCategorie = lees_capacitieve_bodemvochtigheids_categorie(capacitieveBodemvochtigheidsWaarde);                     // capacitieveBodemvochtigheidsCategorie = lees_capacitieve_bodemvochtigheids_categorie(capacitieveBodemvochtigheidsWaarde);
    finaleBodemvochtigheidsCategorie = bereken_finale_bodemvochtigheids_categorie(resistieveBodemvochtigheidsCategorie, capacitieveBodemvochtigheidsCategorie);
    DUMP(resistieveBodemvochtigheidsCategorie);
    DUMP(capacitieveBodemvochtigheidsCategorie);
    DUMP(finaleBodemvochtigheidsCategorie);
    BREAK();

    if (MOCK_SWITCH == LOW) {
      temperatuur = lees_temperatuur();               // temperatuur = lees_temperatuur();
    }
    else {
      temperatuur = get_MOCK_value(TEMPERATURE_SENSOR_MOCK_RANDOM, TEMPERATURE_SENSOR_MOCK_VALUE, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_1, TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_2);
    }
    DUMP(temperatuur);
    BREAK();

    waterGevenTijdsInterval = lees_sensoren_en_geef_water_indien_nodig(finaleBodemvochtigheidsCategorie, temperatuur);
    DUMP(waterGevenTijdsInterval);

    BREAK();
  }
}