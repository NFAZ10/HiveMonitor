#include "var.h"

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);
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

float h1 = NAN, t1 = NAN;
float h2 = NAN, t2 = NAN;
int last_weightstore = 0;
int last_weight = 0;
float calibrationValue = CALIBRATION_FACTOR;
String calibrationStatus = "";
String wifiSSID = "";
String wifiPassword = "";
float voltageDividerReading = 0.0;
float voltageCalibrationFactor = 1.36;
float voltageOffset = 0.0;

float vout = 0.0;
float vin = 0.0;
int value = 0;
unsigned long t = 0;

const char* versionURL = "https://raw.githubusercontent.com/NFAZ10/HiveMonitor/refs/heads/main/src/version.txt";
const char* firmwareURL = "https://raw.githubusercontent.com/NFAZ10/HiveMonitor/main/.pio/build/esp32dev/firmware.bin";
const String currentVersion = "1.0.0";
