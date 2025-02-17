#include <Arduino.h>
#include <OneWire.h>

#include <calibrate_bodemvochtigheidssensoren.h>

#define RESISTANCE_HUMIDITY_SENSOR 36
#define CAPACITIVE_HUMIDITY_SENSOR 39

unsigned long resistanceHumidityValue = 0;
unsigned long capacitanceHumidityValue = 0;

const int TIME_INTERVAL = 2500;
unsigned long timer1 = 0;

void setup() {}

void loop() {}