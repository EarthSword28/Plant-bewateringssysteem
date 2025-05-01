// DONE: Wachttijd tussen 2 opeenvolgende inlezingen van sensoren (in milliseconden)
const int TIJD_INTERVAL_SENSOREN = 5000;
const int PANIC_BUTTON_DEBOUNCE = 500;

// DONE: Temperatuur schakelwaarden (in °C )
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

// Deep Sleep
  // Deep sleep schakelaar, nuttig voor te debuggen om te kijken hoe functies werken zonder in deep sleep te gaan
// const String DEEP_SLEEP_ON = "DEEP SLEEP AAN";
// const String DEEP_SLEEP_OFF = "DEEP SLEEP UIT";

  // Deep Sleep mogelijkheden
const String DEEP_SLEEP_WAKE_UP_START = "STARTUP";
const String DEEP_SLEEP_WAKE_UP_TIME = "DEEP_SLEEP_TIMER";
const String DEEP_SLEEP_WAKE_UP_PANIC_BUTTON = "PANIC_BUTTON";
const String DEEP_SLEEP_WAKE_UP_UNDEFINED = "DEEP_SLEEP_NOT_DEFINED";

// waarschuwing
const String WAARSCHUWING_GEVAAR = "OMGEVALLEN";
const String WAARSCHUWING_OK = "RECHT";

const int WIFI_TOEGESTANE_POGINGEN = 5;
const int WIFI_TOEGESTANE_POGINGEN_HERSTEL = 1;

// WIFI
  // NTP = Network Time Protocol
const long GMT_OFFSET_SEC = 0; // 19800;  // GMT time offset in seconden
const int DAYLIGHT_OFFSET_SEC = 0;        // Daylight offset in seconden


// acce
/* 
const int DOWN_X = DFRobot_LIS2DW12::eXDown;
const int UP_X = DFRobot_LIS2DW12::eXUp;
const int DOWN_Y = DFRobot_LIS2DW12::eYDown;
const int UP_Y = DFRobot_LIS2DW12::eYUp;
const int DOWN_Z = DFRobot_LIS2DW12::eZDown;
const int UP_Z = DFRobot_LIS2DW12::eZUp; 
*/
const int STANDAARD_ORIENTATIE = DFRobot_LIS2DW12::eYUp;

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
// const int TRACE_SCHAKELAAR = 1;  // schakel alle trace-commando's aan(1)/uit(0): Dit werkt alleen als het in het main.cpp bestand staat, niet als het in het config bestand staat.
const bool I2C_SCHAKELAAR = HIGH;   // Schakel de I2C verbinding in(HIGH)/uit(LOW)   Toegevoegd omdat de accelerometer plots stopte met werken
const String WAARSCHUWING_INTIALISATIE = WAARSCHUWING_OK;    // wat is de default waarde van de waarschuwings constante ("GEVAAR"/"OK"), nuttig bij debuggen

// MOCK
const bool MOCK_SCHAKELAAR = LOW;   // schakel alle mock waarden aan(HIGH)/uit(LOW)

const bool CAPACITIEVE_SENSOR_MOCK_RANDOM_SCHAKELAAR = LOW;  // schakel alle willekeurige mock waarden aan(HIGH)/uit(LOW)
const int CAPACITIEVE_SENSOR_MOCK_WAARDE = 1000;
const int CAPACITIEVE_SENSOR_MOCK_RANDOM_WAARDE_1 = 0;
const int CAPACITIEVE_SENSOR_MOCK_RANDOM_WAARDE_2 = 4095;

const bool RESISTIEVE_SENSOR_MOCK_RANDOM_SCHAKELAAR = LOW;  // schakel alle willekeurige mock waarden aan(HIGH)/uit(LOW)
const int RESISTIEVE_SENSOR_MOCK_WAARDE = 4000;
const int RESISTIEVE_SENSOR_MOCK_RANDOM_WAARDE_1 = 0;
const int RESISTIEVE_SENSOR_MOCK_RANDOM_WAARDE_2 = 4095;

const bool TEMPERATUUR_SENSOR_MOCK_RANDOM_SCHAKELAAR = LOW;  // schakel alle willekeurige mock waarden aan(HIGH)/uit(LOW)
const int TEMPERATUUR_SENSOR_MOCK_WAARDE = 30;
const int TEMPERATUUR_SENSOR_MOCK_RANDOM_WAARDE_1 = 0;
const int TEMPERATUUR_SENSOR_MOCK_RANDOM_WAARDE_2 = 30;

  // veranderingen
// ik was vergeten om dit bestand te uploaden voor de eerste deadline, met als gevolg dat er geen veranderingen zijn