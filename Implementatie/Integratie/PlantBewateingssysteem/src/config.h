// DONE: Wachttijd tussen 2 opeenvolgende inlezingen van sensoren
const int TIME_INTERVAL_SENSORS = 5000;

// DONE: Temperatuur schakelwaarden
const int MIN_TEMPERATUUR = 5;
const int MAX_TEMPERATUUR = 25;

// DONE: Categoriën vochtigheid
const char HUMIDITY_NO_VALUE[10] = "NO VALUE";
const char HUMIDITY_DRY[10] = "DRY";
const char HUMIDITY_WET[10] = "WET";
const char HUMIDITY_WATER[10] = "WATER";

// DONE: Statussen water geven (geen water, wél water)
const int WATER_NIET_GEVEN = 0;
const int WATER_GEVEN = 1;

// DONE: Duurtijden water geven (in milliseconden)
const int WATERING_TIME_INTERVAL_INACTIVE = 0;
const int WATERING_TIME_INTERVAL_SHORT = 1000;
const int WATERING_TIME_INTERVAL_LONG = 2000;

// Intervallen voor BVH waarden
// Opgelet, we definiëren de intervallen als gesloten: [min, max]
// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "DROOG" te krijgen
const int RESISTIEVE_SENSOR_DROOG_INTERVAL_MIN = 0;
const int RESISTIEVE_SENSOR_DROOG_INTERVAL_MAX = 2060;

// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "VOCHTIG" te krijgen
const int RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MIN = 2061;
const int RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MAX = 2546;

// Minimum- en maximumwaarde voor de resistieve vochtigheidssensor om de interpretatie "NAT" te krijgen
const int RESISTIEVE_SENSOR_NAT_INTERVAL_MIN = 2547;
const int RESISTIEVE_SENSOR_NAT_INTERVAL_MAX = 4095;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "DROOG" te krijgen
const int CAPACITIEVE_SENSOR_DROOG_INTERVAL_MIN = 2872;
const int CAPACITIEVE_SENSOR_DROOG_INTERVAL_MAX = 4095;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "VOCHTIG" te krijgen
const int CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MIN = 2464;
const int CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MAX = 2871;

// Minimum- en maximumwaarde voor de capacitieve vochtigheidssensor om de interpretatie "NAT" te krijgen
const int CAPACITIEVE_SENSOR_NAT_INTERVAL_MIN = 0;
const int CAPACITIEVE_SENSOR_NAT_INTERVAL_MAX = 2463;