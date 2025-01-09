#ifndef VAR_H
#define VAR_H

#include "globals.h"

// Declare global variables as extern
extern WiFiClientSecure secureClient;
extern PubSubClient mqttClient;
extern Preferences prefs;

extern DHT dht1;
extern DHT dht2;
extern HX711_ADC LoadCell;

extern bool debug;
extern bool tareRequested;
extern bool lowPowerMode;
extern bool useArduinoCloud;

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

extern const char* versionURL;
extern const char* firmwareURL;
extern const String currentVersion;

#endif
