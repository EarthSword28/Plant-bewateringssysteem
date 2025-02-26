// Minimum- en maximumvalue of the resistance humidity sensor to recieve the interpretation "DRY"
const int RESISTANCE_SENSOR_DRY_INTERVAL_MIN = 0;
const int RESISTANCE_SENSOR_DRY_INTERVAL_MAX = 2060;

// Minimum- en maximumvalue of the resistance humidity sensor to recieve the interpretation "WET"
const int RESISTANCE_SENSOR_WET_INTERVAL_MIN = 2061;
const int RESISTANCE_SENSOR_WET_INTERVAL_MAX = 2546;

// Minimum- en maximumvalue of the resistance humidity sensor to recieve the interpretation "WATER"
const int RESISTANCE_SENSOR_WATER_INTERVAL_MIN = 2547;
const int RESISTANCE_SENSOR_WATER_INTERVAL_MAX = 4095;
 
// Minimum- en maximumvalue of the capacitance humidity sensor to recieve the interpretation "DRY"
const int CAPACITANCE_SENSOR_DRY_INTERVAL_MIN = 2872;
const int CAPACITANCE_SENSOR_DRY_INTERVAL_MAX = 4095;

// Minimum- en maximumvalue of the capacitance humidity sensor to recieve the interpretation "WET"
const int CAPACITANCE_SENSOR_WET_INTERVAL_MIN = 2464;
const int CAPACITANCE_SENSOR_WET_INTERVAL_MAX = 2871;

// Minimum- en maximumvalue of the capacitance humidity sensor to recieve the interpretation "WATER"
const int CAPACITANCE_SENSOR_WATER_INTERVAL_MIN = 0;
const int CAPACITANCE_SENSOR_WATER_INTERVAL_MAX = 2463;

// Legenda humidity
const short HUMIDITY_NO_VALUE = 0;
const short HUMIDITY_DRY = 1;
const short HUMIDITY_WET = 2;
const short HUMIDITY_WATER = 3;

// Timers
const int TIME_INTERVAL_SENSORS = 5000;
const int PANIC_BUTTON_DEBOUNCE = 500;

  // watering timers
const int WATERING_TIME_INTERVAL_INACTIVE = 0;
const int WATERING_TIME_INTERVAL_SHORT = 1000;
const int WATERING_TIME_INTERVAL_LONG = 2000;
const int WATERING_TIME_INTERVAL_PANIC_BUTTON = 5000;

// Temperature intervals
const int MIN_TEMPERATURE = 5;
const int MAX_TEMPERATURE = 25;

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