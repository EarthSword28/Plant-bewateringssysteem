#include <WString.h>
#include <esp32-hal-gpio.h>
// DONE: Wachttijd tussen 2 opeenvolgende inlezingen van sensoren
const int TIME_INTERVAL_SENSORS = 5000;
const int PANIC_BUTTON_DEBOUNCE = 500;

// DONE: Temperatuur schakelwaarden
const int MIN_TEMPERATURE = 5;
const int MAX_TEMPERATURE = 25;

// DONE: Categoriën vochtigheid
const String HUMIDITY_NO_VALUE = "GEEN METING";
const String HUMIDITY_DRY = "DROOG";
const String HUMIDITY_WET = "VOCHTIG";
const String HUMIDITY_WATER = "NAT";

// DONE: Statussen water geven (geen water, wél water)
const String GEEN_WATER_GEVEN = "GEEN WATER GEVEN";
const String WATER_GEVEN = "WATER GEVEN";

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
const bool MOCK_SWITCH = LOW;   // Enable(HIGH)/Disable(LOW) all mock values

const bool CAPACITANCE_SENSOR_MOCK_RANDOM = LOW;  // Enable(HIGH)/Disable(LOW) all random mock values
const int CAPACITANCE_SENSOR_MOCK_VALUE = 0;
const int CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int CAPACITANCE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;

const bool RESISTANCEE_SENSOR_MOCK_RANDOM = LOW;  // Enable(HIGH)/Disable(LOW) all random mock values
const int RESISTANCE_SENSOR_MOCK_VALUE = 0;
const int RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int RESISTANCE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;

const bool TEMPERATURE_SENSOR_MOCK_RANDOM = LOW;  // Enable(HIGH)/Disable(LOW) all random mock values
const int TEMPERATURE_SENSOR_MOCK_VALUE = 0;
const int TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_1 = 0;
const int TEMPERATURE_SENSOR_MOCK_RANDOM_VALUE_2 = 0;