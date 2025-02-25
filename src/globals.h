#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <pgmspace.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Preferences.h>
//#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Adafruit_Sensor.h>
#include "HX711_ADC.h"
#include "DHT.h"
#include "ArduinoJson.h"
#include "Adafruit_NeoPixel.h"

// External declarations for global variables
//extern WiFiClientSecure secureClient;
extern PubSubClient mqttClient;
extern Preferences prefs;

// DHT & Scale
extern DHT dht1;
extern DHT dht2;
extern HX711_ADC LoadCell;

// AWS Topics
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// Constants and definitions
#define TARE_BUTTON_PIN        25
#define VOLTAGE_PIN            32 // GPIO pin connected to the voltage divider
#define NUM_SAMPLES            500
#define MAX_DATA_POINTS        144
#define CALIBRATION_FACTOR     29.17 // Default calibration factor
#define SLEEP_INTERVAL_LOW_POWER 21600 // 6 hours
#define SLEEP_INTERVAL_NORMAL    3600  // 1 hour
#define LED_PIN    12

#define LED_COUNT 1

// Declare our NeoPixel strip object:


// Global flags and settings
extern bool debug;
extern bool tareRequested;
extern bool lowPowerMode;
extern bool useArduinoCloud;
extern bool weightset;
// Global variables
extern float battery;
extern float grams;
extern float temp1;
extern float temp2;
extern float weight;
extern int counter;
extern int mVA;
extern float weightInPounds;

extern float h1, t1;
extern float h2, t2;
extern int last_weightstore;
extern int last_weight;
extern float calibrationValue;
extern String calibrationStatus;
extern String wifiSSID;
extern String wifiPassword;
extern float voltageDividerReading;
extern float voltageCalibrationFactor;
extern float voltageOffset;

extern float vout;
extern float vin;
extern int value;
extern unsigned long t;





#endif
