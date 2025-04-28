// BRONNEN
  // de code voor de temperatuursensor: https://canvas.kdg.be/courses/49816/pages/digitale-temperatuursensor-ds18b20-waterproof?module_item_id=1126867 (01/03/2025)
  // ArduinoTrace: https://github.com/bblanchon/ArduinoTrace (01/03/2025)
  // leren werken met strings: https://docs.arduino.cc/language-reference/en/variables/data-types/stringObject/?_gl=1*mcu2lh*_up*MQ..*_ga*MTUzMzU4NTIyNS4xNzQwNTY2MTI2*_ga_NEXN8H46L5*MTc0MDU2NjEyNS4xLjEuMTc0MDU2NjU2Mi4wLjAuMjExOTM3Njc3Nw.. (01/03/2025)
  // code layout: https://canvas.kdg.be/courses/49816/pages/basisstructuur-code-en-configuratie-van-het-plantbewateringssysteem?module_item_id=1168940 (01/03/2025)

  // code referentie: https://canvas.kdg.be/courses/49816 (01/03/2025)

  // DEEP SLEEP met behulp van EXT0: https://www.programmingelectronics.com/external-wake-up-esp32/ (31/03/2025)

  // I2C/accelerometer: https://github.com/DFRobot/DFRobot_LIS
    // accelerometer orientatie: https://github.com/DFRobot/DFRobot_LIS/blob/master/examples/LIS2DW12/orientation/orientation.ino
  

/**！
 * @file activityDetect.ino
 * @brief Motion detection, can detect whether the module is moving
 * @n It’s necessary to go into low power mode before using this function. Then call setActMode() to make the chip in sleep mode. 
 * @n In this state, the measurement rate is 12.5hz.
 * @n When the acceleration change in a certain direction is detected to exceed the threshold, the measurement rate will be increased 
 * @n to the normal rate we set before. The threshold can be set by the setWakeUpThreshold() function.
 * @n But if the move stops moving, also, the acceleration change in the three directions is less than the threshold, the chip will turn into sleep
 * @n mode after a period of time. This duration time can be set by the setWakeUpDur() function.
 * @n When using SPI, chip select pin can be modified by changing the value of LIS2DW12_CS.
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2021-01-16
 * @url https://github.com/DFRobot/DFRobot_LIS
 */

#include <DFRobot_LIS2DW12.h>

//When using I2C communication, use the following program to construct an object by DFRobot_LIS2DW12_I2C
/*!
 * @brief Constructor 
 * @param pWire I2c controller
 * @param addr  I2C address(0x18/0x19)
 */
//DFRobot_LIS2DW12_I2C acce(&Wire,0x18);
DFRobot_LIS2DW12_I2C acce;

int huidigeOrientatie = 0;

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>
#include <security.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

// deep sleep problemen oplossen
#include <esp_system.h>
#include <esp_sleep.h>

#define ARDUINOTRACE_ENABLE 1  // schakel alle trace-commando's aan(1)/uit(0)
#include <ArduinoTrace.h>

// DONE: Definieer juiste pinnummers voor sensoren
#define RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR 36
#define CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR 39

#define ONE_WIRE_BUS 4                   // temperatuur sensor

#define PANIC_BUTTON 25                   // gebruik GPIO 25 als de panic button

#define RELAY_MODULE 17                   // de relay voor de pomp

#define POWER 26        // De power switch voor de sensoren
 
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);    

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);


// DONE: Variabelen om wachttijd tussen inlezen sensoren te kunnen regelen
unsigned long timer = 0;
unsigned long sleepTimer = 0;
unsigned long panicButtonDebounceTimer = 0;

// DONE: Variabelen om duurtijd van water geven te kunnen regelen
unsigned long waterTimer = 0;
unsigned long waterGevenTijdsInterval = 0;

// DONE: Variabele om status van de waterpomp aan te geven, dit is nodig om te kunnen controlleren of de waterpomp gestopt moet worden
String waterStatus = GEEN_WATER_GEVEN;
boolean pompSchakelaar;

boolean panicButtonSchakelaar;

String waarschuwing = WAARSCHUWING_INTIALISATIE;

/*
  placeholder source
*/
#include <driver/rtc_io.h>

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)  // 2 ^ GPIO_NUMBER in hex
#define WAKEUP_GPIO GPIO_NUM_25

#define uS_TO_mS_FACTOR 1000                     /* Conversion factor for micro seconds to milli seconds */
#define TIME_TO_SLEEP  TIJD_INTERVAL_SENSOREN   /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int standaardOrientatie = STANDAARD_ORIENTATIE;
// RTC_DATA_ATTR int loopCount = 0;
boolean dataRedSwitch;

String deepSleepWakeUpReason = "";

// WIFI
  // NTP = Network Time Protocol
const char *NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 0; // 19800;
const int DAYLIGHT_OFFSET_SEC = 0;

  // Google Apps Script URL
const String GOOGLE_APPS_SCRIPT_URL = "https://script.google.com/macros/s/";

void initWifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  Serial.flush();

  int wifiPogingen = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED && wifiPogingen < WIFI_TOEGESTANE_POGINGEN) {
    delay(500);
    Serial.print(".");
    ++wifiPogingen;
  }
}

void setTimezone(String timezone) {
  Serial.printf("Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1);
  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

String getCurrentDateAndTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))   {
    Serial.println("Failed to obtain time");
    return "";
  }

  char timeStringBuff[50]; // 50 chars should be enough
  //strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d-%m-%Y_%H:%M:%S", &timeinfo);

  String asString(timeStringBuff);
  asString.replace(" ", "-");

  return asString;
}

void send_data(int dataTemperatuur, String dataResistieveSensor, String dataCapacitieveSensor, String dataBodemvochtigheidFinaal, int dataWaterTijd, String dataOrientatie) {
  if (WiFi.status() == WL_CONNECTED) {
    // Get current date and time
    String currentDateAndTime = getCurrentDateAndTime();
    Serial.print("Current date and time: ");
    Serial.println(currentDateAndTime);

    // Create URL with parameters to call Google Apps Script
    String urlFinal = GOOGLE_APPS_SCRIPT_URL + GOOGLE_SCRIPT_DEPLOYMENT_ID + "/exec?" + 
        "datum_tijdstip=" + currentDateAndTime + 
        "&temperatuur=" + dataTemperatuur + 
        "&resistieve_sensor=" + dataResistieveSensor +
        "&capacitieve_sensor=" + dataCapacitieveSensor +
        "&bodemvochtigheid_finaal=" + dataBodemvochtigheidFinaal +
        "&water_tijd=" + dataWaterTijd + 
        "&orientatie=" + dataOrientatie;

    Serial.print("POST data to spreadsheet: ");
    Serial.println(urlFinal);

    // Send HTTP request and get status code
    HTTPClient http;
    http.begin(urlFinal.c_str());
    // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);

    // Get response from HTTP request
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    http.end();
  }
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void get_wakeup_reason() {
  TRACE();
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Wakeup caused by external signal using RTC_IO");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_PANIC_BUTTON;
  }
  else if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_UNDEFINED;
  }
  else if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Wakeup caused by timer");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_TIME;
  }
  else if (wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD) {
    Serial.println("Wakeup caused by touchpad");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_UNDEFINED;
  }
  else if (wakeup_reason == ESP_SLEEP_WAKEUP_ULP) {
    Serial.println("Wakeup caused by ULP program");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_UNDEFINED;
  }
  else if (bootCount == 1) {
    Serial.println("Startup");
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_START;
  }
  else {
    Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason);
    deepSleepWakeUpReason = DEEP_SLEEP_WAKE_UP_UNDEFINED;
  }
}

void activate_deep_sleep(int currentTime) {
  TRACE();
  Serial.println("Preparing to sleep");

  // 1. Zet de accelerometer in power-down mode
  acce.setDataRate(DFRobot_LIS2DW12::eRate_0hz);

  delay(50);

  // 2. Sluit de I2C bus af
  Wire.end();

  // 3. Zet SDA en SCL in veilige toestand
  pinMode(21, INPUT_PULLUP); // SDA pin
  pinMode(22, INPUT_PULLUP); // SCL pin

  delay(100);

  // deactiveer de WiFi verbinfing
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

  digitalWrite(POWER, HIGH);

  delay (50);

  if (currentTime < (TIJD_INTERVAL_SENSOREN - 500)) {
    sleepTimer = TIJD_INTERVAL_SENSOREN - currentTime;
  }
  else {
    sleepTimer = TIJD_INTERVAL_SENSOREN;
  }
  Serial.print("Going to sleep now for: ");
  Serial.println(sleepTimer);
  esp_sleep_enable_timer_wakeup(sleepTimer * uS_TO_mS_FACTOR);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void acce_setup() {
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
  //Software reset
  acce.softReset();
  
  /**！
     Set the sensor measurement range:
                    e2_g   /<±2g>/
                    e4_g   /<±4g>/
                    e8_g   /<±8g>/
                    e16_g  /<±16g>/
  */
  acce.setRange(DFRobot_LIS2DW12::e2_g);

  /**！
     Filter settings:
            eLPF(Low pass filter)
            eHPF(High pass filter)
  */
  acce.setFilterPath(DFRobot_LIS2DW12::eLPF);
  
  /**！
     Set bandwidth：
        eRateDiv_2  ,/<Rate/2 (up to Rate = 800 Hz, 400 Hz when Rate = 1600 Hz)>/
        eRateDiv_4  ,/<Rate/4 (High Power/Low power)>*
        eRateDiv_10 ,/<Rate/10 (HP/LP)>/
        eRateDiv_20 ,/< Rate/20 (HP/LP)>/
  */
  acce.setFilterBandwidth(DFRobot_LIS2DW12::eRateDiv_4);
  
  /**
      Wake-up duration: when using the detection mode of eDetectAct in the setActMode() function, it will collect data
    at a normal rate after the chip is awakened. Then after a period of time, the chip will continue to hibernate, collecting data at a frequency of 12.5hz.
    dur (0 ~ 3)
    time = dur * (1/Rate)(unit:s)
    |                    An example of a linear relationship between an argument and time                                    |
    |------------------------------------------------------------------------------------------------------------------------|
    |                |                     |                          |                          |                           |
    |  Data rate     |       25 Hz         |         100 Hz           |          400 Hz          |         = 800 Hz          |
    |------------------------------------------------------------------------------------------------------------------------|
    |   time         |dur*(1s/25)= dur*40ms|  dur*(1s/100)= dur*10ms  |  dur*(1s/400)= dur*2.5ms |  dur*(1s/800)= dur*1.25ms |
    |------------------------------------------------------------------------------------------------------------------------|
    */
  acce.setWakeUpDur(/*dur = */2);
  
  //Set wakeup threshold, when the acceleration change exceeds this value, the eWakeUp event will be triggered, unit:mg
  //The value is within the range.
  acce.setWakeUpThreshold(/*threshold = */0.2);
  
  /**！
    Set power mode:
        eHighPerformance_14bit         /<High-Performance Mode,14-bit resolution>/
        eContLowPwr4_14bit             /<Continuous measurement,Low-Power Mode 4(14-bit resolution)>/
        eContLowPwr3_14bit             /<Continuous measurement,Low-Power Mode 3(14-bit resolution)>/
        eContLowPwr2_14bit             /<Continuous measurement,Low-Power Mode 2(14-bit resolution)/
        eContLowPwr1_12bit             /<Continuous measurement,Low-Power Mode 1(12-bit resolution)>/
        eSingleLowPwr4_14bit           /<Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution)>/
        eSingleLowPwr3_14bit           /<Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution)>/
        eSingleLowPwr2_14bit           /<Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution)>/
        eSingleLowPwr1_12bit           /<Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution)>/
        eHighPerformanceLowNoise_14bit /<High-Performance Mode,Low-noise enabled,14-bit resolution>/
        eContLowPwrLowNoise4_14bit     /<Continuous measurement,Low-Power Mode 4(14-bit resolution,Low-noise enabled)>/
        eContLowPwrLowNoise3_14bit     /<Continuous measurement,Low-Power Mode 3(14-bit resolution,Low-noise enabled)>/
        eContLowPwrLowNoise2_14bit     /<Continuous measurement,Low-Power Mode 2(14-bit resolution,Low-noise enabled)>/
        eContLowPwrLowNoise1_12bit     /<Continuous measurement,Low-Power Mode 1(12-bit resolution,Low-noise enabled)>/
        eSingleLowPwrLowNoise4_14bit   /<Single data conversion on demand mode,Low-Power Mode 4(14-bit resolution),Low-noise enabled>/
        eSingleLowPwrLowNoise3_14bit   /<Single data conversion on demand mode,Low-Power Mode 3(14-bit resolution),Low-noise enabled>/
        eSingleLowPwrLowNoise2_14bit   /<Single data conversion on demand mode,Low-Power Mode 2(14-bit resolution),Low-noise enabled>/
        eSingleLowPwrLowNoise1_12bit   /<Single data conversion on demand mode,Low-Power Mode 1(12-bit resolution),Low-noise enabled>/
  */
  acce.setPowerMode(DFRobot_LIS2DW12::eContLowPwrLowNoise1_12bit);
  
  /**！
     Set the mode of motion detection:
    eNoDetection       /<No detection>/
    eDetectAct         /<If set this mode, the rate of the chip will drop to 12.5hz and turn into normal measurement frequency 
                        after the eWakeUp event is generated.>/
    eDetectStatMotion  /<In this mode, it can only detect if the chip is in sleep mode without changing the measurement frequency
                          and power mode, continuously measuring the data at normal frequency.>/
  */
  acce.setActMode(DFRobot_LIS2DW12::eDetectAct);
  
  /**！
     Set the interrupt source of the int1 pin:
    eDoubleTap(Double click)
    eFreeFall(Free fall)
    eWakeUp(wake up)
    eSingleTap(single-Click)
    e6D(Orientation change check)
  */
  acce.setInt1Event(DFRobot_LIS2DW12::eWakeUp);
  
  /**！
     Set the sensor data collection rate:
                eRate_0hz           /<Measurement off>/
                eRate_1hz6          /<1.6hz, use only under low-power mode>/
                eRate_12hz5         /<12.5hz>/
                eRate_25hz          
                eRate_50hz          
                eRate_100hz         
                eRate_200hz         
                eRate_400hz       /<Use only under High-Performance mode>/
                eRate_800hz       /<Use only under High-Performance mode>/
                eRate_1k6hz       /<Use only under High-Performance mode>/
                eSetSwTrig        /<The software triggers a single measurement>/
  */
  acce.setDataRate(DFRobot_LIS2DW12::eRate_200hz);
}

/**
 * Bepaal de temperatuur, op basis van de gekozen temperatuursensor.
 * Voor een digitale sensor zal dit anders zijn dan voor een analoge.
 * Geeft de temperatuur in °C terug.
 */
float leesTemperatuur() {
  TRACE();
  // DONE: Implementeer zodat de temperatuur op de juiste manier wordt ingelezen
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  //return the temperature in Celsius
  return sensors.getTempCByIndex(0);
}

/**
 * Bepaal de juiste sensorwaarde voor de capacitieve bodemvochtigheidssensor.
 */
int leesCapacitieveBVHSensor() {
  TRACE();
  // DONE: Implementeer inlezen met correcte pinnen
  return analogRead(CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR);
}

/**
 * Bepaal de juiste sensorwaarde voor de resistieve bodemvochtigheidssensor.
 */
int leesResistieveBVHSensor() {
  TRACE();
  // DONE: Implementeer inlezen met correcte pinnen
  return analogRead(RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR);
}

/* MOCK functies om sensoren te bypassen */
int leesTemperatuur_MOCK(boolean randomSchakelaar, int waarde, int randomWaarde1, int randomWaarde2) {
  TRACE();
  if (randomSchakelaar == LOW) {
    return waarde;
  }
  else {
    // Return random waarde tussen 0 en 30 °C
    return random(randomWaarde1, randomWaarde2);
  }
}

int leesCapacitieveBVHSensor_MOCK(boolean randomSchakelaar, int waarde, int randomWaarde1, int randomWaarde2) {
  TRACE();
  if (randomSchakelaar == LOW) {
    return waarde;
  }
  else {
    // Return random waarde tussen 0 en 4095
    return random(randomWaarde1, randomWaarde2);
  }
}

int leesResistieveBVHSensor_MOCK(boolean randomSchakelaar, int waarde, int randomWaarde1, int randomWaarde2) {
  TRACE();
  if (randomSchakelaar == LOW) {
    return waarde;
  }
  else {
    // Return random waarde tussen 0 en 4095
    return random(randomWaarde1, randomWaarde2);
  }
}

/**
 * Bepaal de categorie van de capacitieve bodemvochtigheidssensor voor de gemeten sensorwaarde.
 * We gebruiken hierbij de configuratie uit onze calibratie.  Per categorie checken we of de waarde
 * tussen de MIN en de MAX valt.
 * Opgelet!!  Gebruik enkel de categoriën uit je configuratiebestand!
 * Pas het type van de return value in het functievoorschrift aan op basis van je configuratiebestand.
 */
String berekenCategorieCapactieveBHV(int sensorwaarde) {
  // DONE: Implementeer zodat de categorie voor de resistieve BVH sensor wordt berekend.
  TRACE();
  if (sensorwaarde >= CAPACITIEVE_SENSOR_DROOG_INTERVAL_MIN && sensorwaarde < CAPACITIEVE_SENSOR_DROOG_INTERVAL_MAX) {
    return VOCHTIGHEID_DROOG;
  }
  else if (sensorwaarde >= CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MIN && sensorwaarde < CAPACITIEVE_SENSOR_VOCHTIG_INTERVAL_MAX) {
    return VOCHTIGHEID_VOCHTIG;
  }
  else if (sensorwaarde >= CAPACITIEVE_SENSOR_NAT_INTERVAL_MIN && sensorwaarde <= CAPACITIEVE_SENSOR_NAT_INTERVAL_MAX) {
    return VOCHTIGHEID_NAT;
  }
  else {
    return VOCHTIGHEID_GEEN_METING;
  }
}

/**
 * Bepaal de categorie van de resistieve bodemvochtigheidssensor voor de gemeten sensorwaarde.
 * We gebruiken hierbij de configuratie uit onze calibratie.  Per categorie checken we of de waarde
 * tussen de MIN en de MAX valt.
 * Opgelet!!  Gebruik enkel de categoriën uit je configuratiebestand!
 * Pas het type van de return value in het functievoorschrift aan op basis van je configuratiebestand.
 */
String berekenCategorieResistieveBVH(int sensorwaarde) {
  // DONE: Implementeer zodat de categorie voor de capacitieve BVH sensor wordt berekend.
  TRACE();
  if (sensorwaarde >= RESISTIEVE_SENSOR_DROOG_INTERVAL_MIN && sensorwaarde < RESISTIEVE_SENSOR_DROOG_INTERVAL_MAX) {
    return VOCHTIGHEID_DROOG;
  }
  else if (sensorwaarde >= RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MIN && sensorwaarde < RESISTIEVE_SENSOR_VOCHTIG_INTERVAL_MAX) {
    return VOCHTIGHEID_VOCHTIG;
  }
  else if (sensorwaarde >= RESISTIEVE_SENSOR_NAT_INTERVAL_MIN && sensorwaarde <= RESISTIEVE_SENSOR_NAT_INTERVAL_MAX) {
    return VOCHTIGHEID_NAT;
  }
  else {
    return VOCHTIGHEID_GEEN_METING;
  }
}

/**
 * Bereken de samengestelde categorie voor beide bodemvochtigheidssensoren.
 * Mogelijke strategiën: droogste wint altijd / één wint altijd / geen mogelijke categorie bij verschil
 * Opgelet!!  Gebruik enkel de categoriën uit je configuratiebestand!
 * Pas het type van de return value in het functievoorschrift aan op basis van je configuratiebestand.
 */
String berekenSamengesteldeCategorie(String categorieResistieveBVH, String categorieCapacitieveBVH) {
  // DONE: Implementeer zodat een samengstelde categorie wordt berekend.  Documenteer de strategie!
  TRACE();
  // als minstens één van beide categoriën droog is geef vochtigheidscategorie "droog" weer
  if (categorieCapacitieveBVH == VOCHTIGHEID_DROOG || categorieResistieveBVH == VOCHTIGHEID_DROOG) {
    return VOCHTIGHEID_DROOG;
  }
  // als geen van beide categoriën droog is en minstens één van beide categoriën vochtig is geef vochtigheidscategorie "vochtig" weer
  else if (categorieCapacitieveBVH == VOCHTIGHEID_VOCHTIG || categorieResistieveBVH == VOCHTIGHEID_VOCHTIG) {
    return VOCHTIGHEID_VOCHTIG;
  }
  // als geen van beide categoriën vochtig is en minstens één van beide categoriën nat is geef vochtigheidscategorie "nat" weer
  else if (categorieCapacitieveBVH == VOCHTIGHEID_NAT || categorieResistieveBVH == VOCHTIGHEID_NAT) {
  return VOCHTIGHEID_NAT;
  }
  // els geen van beide categoriën een bruikbare categorie is geef weer dat er geen correcte meting is uitgevoerd
  else {
    return VOCHTIGHEID_GEEN_METING;
  }
}

/**
 * Zet de waterpomp aan voor een bepaalde tijd.   
 * Opgelet!!  Deze functie mag GEEN DELAY bevatten.  De duurtijd zal dus via een variabele moeten bijgehouden worden.
 *            Het hoofdprogramma moet telkens controlleren of de duurtijd reeds verstreken is, via millis().
 *            Gebruik een status om aan te geven dat de waterpomp aan het water geven is.
 */
void zetWaterpompAan(int duurtijd) {
  TRACE();
  if (waarschuwing == WAARSCHUWING_OK) {
    // DONE: Implementeer code om de pomp aan te zetten
    digitalWrite(RELAY_MODULE, LOW);

    // DONE: Initialiseer de variabelen om de starttijd en duurtijd van het water geven te regelen
    waterTimer = millis() + duurtijd;
    waterStatus = WATER_GEVEN;

    DUMP(RELAY_MODULE);
    DUMP(millis());
    DUMP(duurtijd);
    DUMP(waterTimer);
  }
}

/**
 * Zet de waterpomp uit. 
 * Opgelet!! Aangezien de zetWaterpompAan() functie geen delay bevat, zullen de variabelen die daar gebruikt worden
 *           opnieuw geïnitialiseerd moeten worden bij het uitzetten van de pomp.
 */
void zetWaterpompUit() {
  TRACE();
  // DONE: Implementeer code om de pomp uit te zetten
  digitalWrite(RELAY_MODULE, HIGH);
  
  // DONE: Initialiseer de variabelen om de starrtijd en duurtijd van het water geven te regelen
  waterStatus = GEEN_WATER_GEVEN;
  waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_INACTIEF;
}

/**
 * Deze functie bevat alle code voor het uitlezen van de sensoren en om de waterpomp indien nodig aan te zetten.
 * Het uitzetten van de waterpomp gebeurt niet hier maar in de loop() functie na controle of er voldoende tijd verstreken is.
 */
void leesSensorenEnGeefWaterIndienNodig() {
  TRACE();
  // DONE: Implementeer inlezen met correcte pinnen
  int capacitieve_bvh_waarde = leesCapacitieveBVHSensor();
  int resistieve_bvh_waarde = leesResistieveBVHSensor();
  int temperatuur = leesTemperatuur();
  DUMP(capacitieve_bvh_waarde);
  DUMP(resistieve_bvh_waarde);
  DUMP(temperatuur);
  if (MOCK_SCHAKELAAR == HIGH) {
    capacitieve_bvh_waarde = leesCapacitieveBVHSensor_MOCK(CAPACITIEVE_SENSOR_MOCK_RANDOM_SCHAKELAAR, CAPACITIEVE_SENSOR_MOCK_WAARDE, CAPACITIEVE_SENSOR_MOCK_RANDOM_WAARDE_1, CAPACITIEVE_SENSOR_MOCK_RANDOM_WAARDE_2);
    resistieve_bvh_waarde = leesResistieveBVHSensor_MOCK(RESISTIEVE_SENSOR_MOCK_RANDOM_SCHAKELAAR, RESISTIEVE_SENSOR_MOCK_WAARDE, RESISTIEVE_SENSOR_MOCK_RANDOM_WAARDE_1, RESISTIEVE_SENSOR_MOCK_RANDOM_WAARDE_2);
    temperatuur = leesTemperatuur_MOCK(TEMPERATUUR_SENSOR_MOCK_RANDOM_SCHAKELAAR, TEMPERATUUR_SENSOR_MOCK_WAARDE, TEMPERATUUR_SENSOR_MOCK_RANDOM_WAARDE_1, TEMPERATUUR_SENSOR_MOCK_RANDOM_WAARDE_2);
    Serial.println("Mock Values");
    DUMP(capacitieve_bvh_waarde);
    DUMP(resistieve_bvh_waarde);
    DUMP(temperatuur);
  }
  //BREAK();
  DUMP("BREAK");

  // Bepaal individuele categoriën en samengestelde categorie
  String categorieCapacitieveBVH = berekenCategorieCapactieveBHV(capacitieve_bvh_waarde);
  String categorieResistieveBVH = berekenCategorieResistieveBVH(resistieve_bvh_waarde);
  String categorie = berekenSamengesteldeCategorie(categorieCapacitieveBVH, categorieResistieveBVH);
  DUMP(categorieCapacitieveBVH);
  DUMP(categorieResistieveBVH);
  DUMP(categorie);
  //BREAK();
  DUMP("BREAK");

  // DONE: Beslis over water geven en pas de controles toe uit de flowchart.  
  // !! Gebruik enkel de constanten uit de configuratie om met een categorie te vergelijken!
  // !! Gebruik enkel de constanten uit de configuratie om de duurtijd van het water geven mee te geven
  // !! Gebruik verder enkel de functies zetWaterpompAan() aan te zetten

  waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_INACTIEF;
  if (categorie == VOCHTIGHEID_DROOG) {
    if (temperatuur > MAX_TEMPERATUUR) {
      waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_LANG;
    }
    else if (temperatuur > MIN_TEMPERATUUR) {
      waterGevenTijdsInterval = WATER_GEVEN_INTERVAL_KORT;
    }
  }

  send_data(temperatuur, categorieResistieveBVH, categorieCapacitieveBVH, categorie, waterGevenTijdsInterval, waarschuwing);

  if (waterGevenTijdsInterval != WATER_GEVEN_INTERVAL_INACTIEF) {
    zetWaterpompAan(waterGevenTijdsInterval);
  }
}

void panic_button() {
  TRACE();
  panicButtonSchakelaar = HIGH;
  panicButtonDebounceTimer = millis() + PANIC_BUTTON_DEBOUNCE;
  zetWaterpompAan(WATER_GEVEN_INTERVAL_PANIC_BUTTON);
  //BREAK();
  DUMP("BREAK");
  timer = millis();
}

// zet alle ongebruikte GPIO pinnen uit, zodat deze geen problemen kunnen veroorzaken
void protect_GPIOs() {
  int pinList[9] = {2, 13, 14, 0, 26, 15, 35, 34, 12};
  for (int i = 0; i <= 8; i++) {
    pinMode(pinList[i], OUTPUT);
    digitalWrite(pinList[i], LOW);
  }
}

void setup() {
  TRACE();
  // DONE: Implementeer de nodig code voor lezen sensoren (indien nodig)
  pinMode(RESISTIEVE_BODEMVOCHTIGHEIDS_SENSOR, INPUT);
  pinMode(CAPACITIEVE_BODEMVOCHTIGHEIDS_SENSOR, INPUT);
  pinMode(ONE_WIRE_BUS, INPUT);
  pinMode(PANIC_BUTTON, INPUT_PULLUP);

  // indien de 2-kanaals 3V relais module gebruikt word moet de relais op High gezet worden om deze uit te zetten, en op Low om deze op te zetten
  pinMode(RELAY_MODULE, OUTPUT);
  digitalWrite(RELAY_MODULE, HIGH);

  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, LOW);

  delay(100);

  protect_GPIOs();

  waterStatus = GEEN_WATER_GEVEN;
  panicButtonSchakelaar = LOW;
  waterGevenTijdsInterval = 0;

  Serial.begin(9600);
  timer = millis();
  waterTimer = millis();
  panicButtonDebounceTimer = millis();
  dataRedSwitch = LOW;

  delay(1000); //Take some time to open up the Serial Monitor

  //Increment boot number and print it every reboot
  Serial.println("Booting...");
  
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  Serial.println(reason);

  ++bootCount;
  Serial.print("Boot number: ");
  Serial.println(bootCount);

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);

  //Print the wakeup reason for ESP32
  get_wakeup_reason();

  esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKEUP_GPIO, 1);  //1 = High, 0 = Low
  // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
  // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
  // No need to keep that power domain explicitly, unlike EXT1.
  rtc_gpio_pullup_dis(WAKEUP_GPIO);
  rtc_gpio_pulldown_en(WAKEUP_GPIO);

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every x seconds
  */
  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_mS_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  // Start up the sensor library
  sensors.begin(); 

  // I2C
  if (I2C_SCHAKELAAR == HIGH) {
    while(!acce.begin()){
      Serial.println("Communication failed, check the connection and I2C address setting when using I2C communication.");
      delay(1000);
    }
    acce_setup();
  }
  else {
    if (acce.begin()) {
      Serial.println("Succesfull I2C connection");
      acce_setup();
    }
    else {
      Serial.println("Communication failed, check the connection and I2C address setting when using I2C communication.");
      Serial.println("I2C-switch is currently turned off");
    }
  }
  delay(100);

  // connect to WiFi
  initWifi();

  // Init NTP and set timezone to Berlin
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  setTimezone("CET-1CEST,M3.5.0,M10.5.0/3");
}

void loop() {
  // We hebben huidige millis nodig om de verschillende processen te controleren (water geven / stoppen)
  long huidigeMillis = millis();

  if (I2C_SCHAKELAAR == HIGH) {
    TRACE();
    huidigeOrientatie = acce.getOrientation();
    if (deepSleepWakeUpReason == DEEP_SLEEP_WAKE_UP_START) {
      standaardOrientatie = huidigeOrientatie;
    }
    else {
      if (huidigeOrientatie == standaardOrientatie) {
        waarschuwing = WAARSCHUWING_OK;
      }
      else {
        waarschuwing = WAARSCHUWING_GEVAAR;
        DUMP(huidigeOrientatie);
        DUMP(standaardOrientatie);
        DUMP(deepSleepWakeUpReason);
      }
    }
    if (dataRedSwitch == LOW) {
      DUMP(huidigeOrientatie);
      DUMP(standaardOrientatie);
      DUMP(deepSleepWakeUpReason);
    }
  }

  // DONE: Controleer of sensoren ingelezen moeten worden en roep functie leesSensorenEnGeefWaterIndienNodig() aan indien nodig
  if (dataRedSwitch == LOW) {
    dataRedSwitch = HIGH;
    if (deepSleepWakeUpReason == DEEP_SLEEP_WAKE_UP_TIME || deepSleepWakeUpReason == DEEP_SLEEP_WAKE_UP_START) {
      TRACE();
      DUMP(huidigeMillis);
      leesSensorenEnGeefWaterIndienNodig();
      //BREAK();
      DUMP("BREAK");
    }
    else if (deepSleepWakeUpReason == DEEP_SLEEP_WAKE_UP_PANIC_BUTTON) {
      TRACE();
      panic_button();
    }
  }
  else if (waterStatus == GEEN_WATER_GEVEN) {
    activate_deep_sleep(huidigeMillis);
  }
  else if (huidigeMillis >= waterTimer) {
    zetWaterpompUit();
  }
}
