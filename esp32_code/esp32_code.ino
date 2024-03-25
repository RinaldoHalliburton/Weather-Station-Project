// LIBRARY IMPORTS
#include <rom/rtc.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <Adafruit_ST7735.h>  // Hardware-specific library
// IMPORT FONTS FOR TFT DISPLAY
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp;

#ifndef _WIFI_H
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

// MQTT CLIENT CONFIG
static const char* pubtopic = "620153775";                         // Add your ID number here
static const char* subtopic[] = { "620153775_sub", "/elet2415" };  // Array of Topics(Strings) to subscribe to
//static const char* mqtt_server = "dbs.msjrealtms.com";          // Broker IP address or Domain name as a String
static const char* mqtt_server = "broker.hivemq.com";          // Broker IP address or Domain name as a String
static uint16_t mqtt_port = 1883;

// WIFI CREDENTIALS
//const char* ssid = "CWC-9128576";   // Add your Wi-Fi ssid
//const char* password = "sb3Rkgqzxtcy";  // Add your Wi-Fi password

const char* ssid = "MonaConnect";   // Add your Wi-Fi ssid
const char* password = "";  // Add your Wi-Fi password

// FUNCTION DECLARATION
void checkHEAP(const char* Name);  // RETURN REMAINING HEAP SIZE FOR A TASK
void initMQTT(void);               // CONFIG AND INITIALIZE MQTT PROTOCOL
unsigned long getTimeStamp(void);  // GET 10 DIGIT TIMESTAMP FOR CURRENT TIME
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char* topic, const char* payload);  // PUBLISH MQTT MESSAGE(PAYLOAD) TO A TOPIC
void vButtonCheck(void* pvParameters);
void vUpdate(void* pvParameters);



// TASK HANDLES
TaskHandle_t xMQTT_Connect = NULL;
TaskHandle_t xNTPHandle = NULL;
TaskHandle_t xLOOPHandle = NULL;
TaskHandle_t xUpdateHandle = NULL;
TaskHandle_t xButtonCheckeHandle = NULL;

//############### IMPORT HEADER FILES ##################
#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

#include <DHT.h>;

//Constants
#define DHTPIN 25  // what pin IT IS connected to
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_ST7735.h>  // Hardware-specific library


#define TFT_DC 17
#define TFT_CS 5
#define TFT_RST 16
#define TFT_CLK 18
#define TFT_MOSI 23
#define TFT_MISO 19
#define CAP 36

/* Initialize class objects*/
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);



uint8_t trigPin = 26;  // Trigger
uint8_t echoPin = 27;  // Echo
float pressure = 0;
int soilmoisture = 0;
float altitude;
float hum;  //Stores humidity value
float heatindex;
float temp;           //Stores temperature value
const int dry = 595;  // value for dry sensor
const int wet = 239;  // value for wet sensor



void setup() {
  Serial.begin(115200);  // INIT SERIAL
  
  bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  pinMode(CAP, INPUT);
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(30, 10);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Weather Station");

  tft.setTextSize(2);

  tft.setTextColor(ILI9341_RED);
  tft.setCursor(0, 60);
  tft.print("Temperatre:");
  tft.print(temp);
  tft.print(" C");

  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(0, 100);
  tft.print("Humidity:");
  tft.print(hum);
  tft.print(" %");

  tft.setTextColor(ILI9341_BLUE);
  tft.setCursor(0, 140);
  tft.print("Heat Index:");
  tft.print(heatindex);
  tft.print(" C");

  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(0, 180);
  tft.print("Pressure:");
  tft.print(pressure);
  tft.print(" kPa");

  tft.setTextColor(ILI9341_ORANGE);
  tft.setCursor(0, 220);
  tft.print("Soil Moisture:");
  tft.print(soilmoisture);
  tft.print(" %");

  tft.setTextColor(ILI9341_PURPLE);
  tft.setCursor(0, 260);
  tft.print("Altitude:");
  tft.print(altitude);
  tft.print(" m");

  dht.begin();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  initialize();            // INIT WIFI, MQTT & NTP
  vButtonCheckFunction();  // UNCOMMENT IF USING BUTTONS THEN ADD LOGIC FOR INTERFACING WITH BUTTONS IN THE vButtonCheck FUNCTION
}

void vButtonCheck(void* pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1);

  for (;;) {

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}




void vUpdate(void* pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1);

  for (;;) {
    // Task code goes here.
    // PUBLISH to topic every second.

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

unsigned long getTimeStamp(void) {
  // RETURNS 10 DIGIT TIMESTAMP REPRESENTING CURRENT TIME
  time_t now;
  time(&now);  // Retrieve time[Timestamp] from system and save to &now variable
  return now;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // ############## MQTT CALLBACK  ######################################
  // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO

  Serial.printf("\nMessage received : ( topic: %s ) \n", topic);
  char* received = new char[length + 1]{ 0 };

  for (int i = 0; i < length; i++) {
    received[i] = (char)payload[i];
  }

  // PRINT RECEIVED MESSAGE
  Serial.printf("Payload : %s \n", received);


  // CONVERT MESSAGE TO JSON


  // PROCESS MESSAGE
}

bool publish(const char* topic, const char* payload) {
  bool res = false;
  try {
    res = mqtt.publish(topic, payload);
    // Serial.printf("\nres : %d\n",res);
    if (!res) {
      res = false;
      throw false;
    }
  } catch (...) {
    Serial.printf("\nError (%d) >> Unable to publish message\n", res);
  }
  return res;
}



double convert_Celsius_to_fahrenheit(double c) {
  // CONVERTS INPUT FROM °C TO °F. RETURN RESULTS
  return (c * 1.8) + 32;
}

double convert_fahrenheit_to_Celsius(double f) {
  // CONVERTS INPUT FROM °F TO °C. RETURN RESULT
  return (f - 32) * 0.555555555;
}


float calcHeatindex(float Temp, float Humid) {
  // CALCULATE AND RETURN HEAT INDEX USING EQUATION FOUND AT
  const double c1 = -42.379;
  const double c2 = 2.04901523;
  const double c3 = 10.14333127;
  const double c4 = -0.22475541;
  const double c5 = -6.83783e-03;
  const double c6 = -5.481717e-02;
  const double c7 = 1.22874e-03;
  const double c8 = 8.5282e-04;
  const double c9 = -1.99e-06;

  double T = convert_Celsius_to_fahrenheit(Temp);
  double RH = Humid;

  double heatIndex = c1 + (c2 * T) + (c3 * RH) + (c4 * T * RH) + (c5 * T * T) + (c6 * RH * RH) + (c7 * T * T * RH) + (c8 * T * RH * RH) + (c9 * T * T * RH * RH);
  double far = convert_fahrenheit_to_Celsius(heatIndex);
   return far;
}

void updateTemp(float new_temp) {
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setCursor(0, 60);
  tft.print("Temperatre:");
  tft.print(new_temp);
  tft.print(" C");
}

void updateHum(float new_hum) {
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setCursor(0, 100);
  tft.print("Humidity:");
  tft.print(new_hum);
  tft.print(" %");
}


void updateHeatindex(float new_heatindex) {

  tft.setTextColor(ILI9341_BLUE, ILI9341_BLACK);
  tft.setCursor(0, 140);
  tft.print("Heat Index:");
  tft.print(new_heatindex);
  tft.print(" C");
}

void updatePressure(float new_pressure, float alt) {

  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor(0, 180);
  tft.print("Pressure:");
  tft.print(new_pressure);
  tft.print(" kPa");

  tft.setTextColor(ILI9341_PURPLE, ILI9341_BLACK);
  tft.setCursor(0, 260);
  tft.print("Altitude:");
  tft.print(alt,1);
  tft.print(" m");


}


void updateSoilMo(int new_soilmoisture) {
  tft.setTextColor(ILI9341_ORANGE,ILI9341_BLACK);
  tft.setCursor(0, 220);
  tft.print("Soil Moisture:");
  tft.print(new_soilmoisture);
  tft.print(" %");
  
}



void loop() {
  // put your main code here, to run repeatedly:
  float new_hum = dht.readHumidity();
  float new_temp = dht.readTemperature();
  float new_heatindex = calcHeatindex(new_temp, new_hum);
  if (temp != new_temp) {
    updateTemp(new_temp);
    temp = new_temp;
  }
  if (hum != new_hum) {
    updateHum(new_hum);
    hum = new_hum;
  }
  if (heatindex != new_heatindex) {
    updateHeatindex(new_heatindex);
    heatindex = new_heatindex;
  }
  
  //float new_pressure = bmp.readPressure()/1000;
  float altitude = abs(bmp.readAltitude(1013.25));
  //float new_pressure = (1014 * pow(1-(altitude/44330),5.255));
  float new_pressure = bmp.readPressure()/1000;
  Serial.println(altitude);
  
  if (pressure != new_pressure) {
    updatePressure(new_pressure,altitude);
    pressure = new_pressure;
  }


  int sensorVal = analogRead(CAP);
 // int new_soilmoisture = map(sensorVal, 1400, 2900, 100, 0);
  int new_soilmoisture = map(sensorVal, 3600, 1600, 0, 100);
  if (new_soilmoisture > 100)
  {
    new_soilmoisture = 100;
  }
  if (new_soilmoisture < 0)
  {
    new_soilmoisture = 0;
  }
  if (soilmoisture != new_soilmoisture) {
    updateSoilMo(new_soilmoisture);
    soilmoisture = new_soilmoisture;
  }
  StaticJsonDocument<768> doc;  // Create JSon object
  char message[800] = { 0 };
  doc["id"] = "620153775";
  doc["timestamp"] = getTimeStamp();
  doc["temperature"] = new_temp;
  doc["humidity"] = new_hum;
  doc["heatindex"] = new_heatindex;
  doc["pressure"] = new_pressure;
  doc["soilmoisture"] = new_soilmoisture;
  doc["altitude"] = altitude;

  serializeJson(doc, message);  // Seralize / Covert JSon object to JSon string and store in char* array
  publish(pubtopic, message);


  delay(1000);
}
