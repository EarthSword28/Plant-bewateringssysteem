// DONE: Wachttijd tussen 2 opeenvolgende inlezingen van sensoren
const int TIJD_INTERVAL_SENSOREN = 5000;
const int PANIC_BUTTON_DEBOUNCE = 500;

// DONE: Temperatuur schakelwaarden
const int MIN_TEMPERATUUR = 5;
const int MAX_TEMPERATUUR = 25;

// DONE: Categoriën vochtigheid
const String VOCHTIGHEID_GEEN_METING = "GEEN METING";
const String VOCHTIGHEID_DROOG = "DROOG";
const String VOCHTIGHEID_VOCHTIG = "VOCHTIG";
const String VOCHTIGHEID_NAT = "NAT";

// DONE: Statussen water geven (geen water, wél water)
const String GEEN_WATER_GEVEN = "GEEN WATER GEVEN";
const String WATER_GEVEN = "WATER GEVEN";

// DONE: Duurtijden water geven (in milliseconden)
const int WATER_GEVEN_INTERVAL_INACTIEF = 0;
const int WATER_GEVEN_INTERVAL_KORT = 1000;
const int WATER_GEVEN_INTERVAL_LANG = 2000;
const int WATER_GEVEN_INTERVAL_PANIC_BUTTON = 5000;

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