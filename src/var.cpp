#include "var.h"

WiFiClientSecure secureClient;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
Preferences prefs;

DHT dht1(27, DHT11);
DHT dht2(26, DHT11);
HX711_ADC LoadCell(5, 18);

bool debug = true;
bool tareRequested = false;
bool lowPowerMode = false;
bool useArduinoCloud = false;

float battery = 0.0;
float grams = 0.0;
float temp1 = NAN;
float temp2 = NAN;
float weight = 0.0;
int counter = 0;
int mVA = 0;

float h1 = -1, t1 = -1;
float h2 = -1, t2 = -1;
int last_weightstore = 0;
int last_weight = 0;
float calibrationValue = CALIBRATION_FACTOR;
String calibrationStatus = "";
String wifiSSID = "";
String wifiPassword = "";
float voltageDividerReading = 0.0;
float voltageCalibrationFactor = 1.31;
float voltageOffset = 0.0;

float vout = 0.0;
float vin = 0.0;
int value = 0;
unsigned long t = 0;

const char* versionURL = "https://raw.githubusercontent.com/NFAZ10/HiveMonitor/refs/heads/main/src/version.txt";
const char* firmwareURL = "https://raw.githubusercontent.com/NFAZ10/HiveMonitor/main/.pio/build/esp32dev/firmware.bin";
<<<<<<< HEAD
const String currentVersion = "1.1.8";
=======
const String currentVersion = "1.1.7";
>>>>>>> 4c22767bc5a8c167c7f14f868b235634b14c793e


// Define the local MQTT server details
const char* mqttServer = "216.220.92.185"; // Replace with your local MQTT server IP address
const int mqttPort = 4116; // Replace with your local MQTT server port
const char* mqttUser = "Hivelogin"; // Replace with your MQTT username
const char* mqttPassword = "824656789"; // Replace with your MQTT password





unsigned long lastPublishTime = 0; // Initialize outside the loop
const unsigned long publishInterval = 30000; // 15 minutes in milliseconds
int awsConnectAttempts = 0;


