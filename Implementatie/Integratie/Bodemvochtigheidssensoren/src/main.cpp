#include <Arduino.h>

#define SENSOR 39

// Arduino: 1023 - 0
// FireBeetle: 4057 - 0

unsigned long CapacitanceMax = 4075;
unsigned long CapacitanceMin = 0;

unsigned long SensorValue = 0;
unsigned long MoisturePercentage = 0;
unsigned long AverageCapacitance = 0;
unsigned long MeanCapacitance = 0;

const int TIME_INTERVAL = 2500;
unsigned long Timer = 0;

int calculateMoisturePercentage(int sensorValue, int sensorMinValue, int sensorMaxValue) {
  return map(sensorValue, sensorMinValue, sensorMaxValue, 0, 100);
}

int calculateAverage(int rawValue, int minValue, int maxValue) {
  return ((rawValue + minValue + maxValue)/3);
}

int calcultateMean(int minValue, int maxValue) {
  return ((minValue + maxValue)/2);
}

void outputSerialSoilValues(int rawValue, int percentage, int minValue, int maxValue, int averageValue, int meanValue) {
  Serial.print("soil sensor value: ");
  Serial.print(rawValue);
  Serial.print(" - Soil moisture percentage: ");
  Serial.print(percentage);
  Serial.println("%");
  Serial.print("Minimum value: ");
  Serial.print(maxValue);
  Serial.print(" - Maximum Value: ");
  Serial.println(minValue);
  Serial.print("Average Value: ");
  Serial.print(averageValue);
  Serial.print("- Mean Value: ");
  Serial.println(meanValue);
}

void setup() {
  pinMode(SENSOR, INPUT);
  Serial.begin(9600);
  Timer = millis();
}

void loop() {
  if (millis() - Timer >= TIME_INTERVAL) {
    Timer = millis();
    SensorValue = analogRead(SENSOR);
    if (SensorValue > CapacitanceMin) {
      CapacitanceMin = SensorValue;
    }
    if (SensorValue < CapacitanceMax) {
      CapacitanceMax = SensorValue;
    }
    MoisturePercentage = calculateMoisturePercentage(SensorValue, CapacitanceMin, CapacitanceMax);
    AverageCapacitance = calculateAverage(SensorValue, CapacitanceMin, CapacitanceMax);
    MeanCapacitance = calcultateMean(CapacitanceMin, CapacitanceMax);
    outputSerialSoilValues(SensorValue, MoisturePercentage, CapacitanceMin, CapacitanceMax, AverageCapacitance, MeanCapacitance);
    Serial.println("--------");
  }
}