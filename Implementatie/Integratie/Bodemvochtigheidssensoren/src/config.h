// DONE: Wachttijd tussen 2 opeenvolgende inlezingen van sensoren
const int TIME_INTERVAL_SENSORS = 5000;
const int PANIC_BUTTON_DEBOUNCE = 500;

// DONE: Temperatuur schakelwaarden
const int MIN_TEMPERATURE = 5;
const int MAX_TEMPERATURE = 25;

// DONE: Categoriën vochtigheid
const char HUMIDITY_NO_VALUE[12] = "GEEN METING";
const char HUMIDITY_DRY[12] = "DROOG";
const char HUMIDITY_WET[12] = "VOCHTIG";
const char HUMIDITY_WATER[12] = "NAT";

// DONE: Statussen water geven (geen water, wél water)
const char GEEN_WATER[11] = "GEEN WATER";
const char WEL_WATER[11] = "WEL WATER";

// DONE: Duurtijden water geven (in milliseconden)
const int WATERING_TIME_INTERVAL_INACTIVE = 0;
const int WATERING_TIME_INTERVAL_SHORT = 1000;
const int WATERING_TIME_INTERVAL_LONG = 2000;
const int WATERING_TIME_INTERVAL_PANIC_BUTTON = 5000;

// Intervallen voor BVH waarden
// Opgelet, we definiëren de intervallen als gesloten: [min, max]
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


// DEBUG
const int TRACE_SWITCH = 1;  // Enable(1)/Disable(0) all traces

  // MOCK
const bool MOCK_SWITCH = 0;   // Enable(HIGH)/Disable(LOW) all mock values

const bool CAPACITANCE_SENSOR_MOCK_RANDOM = 0;  // Enable(HIGH)/Disable(LOW) all random mock values
const int CAPACITANCE_SENSOR_MOCK_VALUE = 0;
const int CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;

const bool RESISTANCEE_SENSOR_MOCK_RANDOM = 0;  // Enable(HIGH)/Disable(LOW) all random mock values
const int RESISTANCE_SENSOR_MOCK_VALUE = 0;
const int RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;

const bool TEMPERATURE_SENSOR_MOCK_RANDOM = 0;  // Enable(HIGH)/Disable(LOW) all random mock values
const int TEMPERATURE_SENSOR_MOCK_VALUE = 0;
const int TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;