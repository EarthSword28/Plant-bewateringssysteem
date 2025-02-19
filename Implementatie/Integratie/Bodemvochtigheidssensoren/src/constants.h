// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "DROOG" te krijgen
const int RESISTANCE_SENSOR_DRY_INTERVAL_MIN = 0;
const int RESISTANCE_SENSOR_DRY_INTERVAL_MAX = 2060;

// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "VOCHTIG" te krijgen
const int RESISTANCE_SENSOR_WET_INTERVAL_MIN = 2061;
const int RESISTANCE_SENSOR_WET_INTERVAL_MAX = 2546;

// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "NAT" te krijgen
const int RESISTANCE_SENSOR_WATER_INTERVAL_MIN = 2547;
const int RESISTANCE_SENSOR_WATER_INTERVAL_MAX = 4095;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "DROOG" te krijgen
const int CAPACITANCE_SENSOR_DRY_INTERVAL_MIN = 2872;
const int CAPACITANCE_SENSOR_DRY_INTERVAL_MAX = 4095;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "VOCHTIG" te krijgen
const int CAPACITANCE_SENSOR_WET_INTERVAL_MIN = 2464;
const int CAPACITANCE_SENSOR_WET_INTERVAL_MAX = 2871;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "NAT" te krijgen
const int CAPACITANCE_SENSOR_WATER_INTERVAL_MIN = 0;
const int CAPACITANCE_SENSOR_WATER_INTERVAL_MAX = 2463;

// Legenda vochtigheid
const int HUMIDITY_NO_VALUE = 0;
const int HUMIDITY_DRY = 1;
const int HUMIDITY_WET = 2;
const int HUMIDITY_WATER = 3;

// Timers
const int TIME_INTERVAL_SENSORS = 5000;

  // water geven timers
const int WATERING_TIME_INTERVAL_SHORT = 1000;
const int WATERING_TIME_INTERVAL_LONG = 2000;

// Temperatuur intervallen
const int MIN_TEMPERATURE = 5;
const int MAX_TEMPERATURE = 25;