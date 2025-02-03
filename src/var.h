#ifndef VAR_H
#define VAR_H

#include "globals.h"

// Declare global variables as extern
extern WiFiClientSecure secureClient;
extern PubSubClient mqttClient;
extern Preferences prefs;

extern Adafruit_NeoPixel strip;


extern DHT dht1;
extern DHT dht2;
extern HX711_ADC LoadCell;

extern bool debug;
extern bool tareRequested;
extern bool lowPowerMode;
extern bool useArduinoCloud;
extern bool weightset;

extern float battery;
extern float grams;
extern float temp1;
extern float temp2;
extern float weight;
extern int counter;
extern int mVA;

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
extern  String otaBranch;
extern const char* MAINversionURL;
extern const char* MAINfirmwareURL;
extern const char* MAINcurrentVersion;
extern const char* DEVversionURL;
extern const char* DEVfirmwareURL;
extern const char* DEVcurrentVersion;


extern unsigned long lastPublishTime; // Initialize outside the loop
extern const unsigned long publishInterval; // 15 minutes in milliseconds
extern int awsConnectAttempts;

extern const char* mqttUser; // Replace with your MQTT username
extern const char* mqttPassword;

extern const char* mqttServer; // Replace with your local MQTT server IP address
extern const int mqttPort; 

extern const char* versionURL;
extern const char* firmwareURL;
extern const char* currentVersion;

#endif
