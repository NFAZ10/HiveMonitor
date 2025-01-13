#include "globals.h"
#include <Arduino.h>
#include "var.h"
#include "secrets.h"
#include "functions.h"


///////////////////////////////////////////////
//               INITIALIZATION
///////////////////////////////////////////////

float movingAverage(int newValue);

void initSerial() {
  if(debug) {
    Serial.begin(9600);
    while (!Serial) ; // Wait for Serial if needed
  }
}

void initPreferences() {
  prefs.begin("beehive-data", false);
}

void loadPreferences() {
  last_weightstore = prefs.getInt("lastWeight", 0);
  wifiSSID         = prefs.getString("wifiSSID", "");
  wifiPassword     = prefs.getString("wifiPassword", "");
  calibrationValue = prefs.getFloat("calibrationFactor", CALIBRATION_FACTOR);
  useArduinoCloud = prefs.getBool("useArduinoCloud", true);


  if(debug) {
    Serial.println(String("Last Weight=") + last_weightstore);
    Serial.println(String("Stored SSID=") + wifiSSID);
    Serial.println(String("Stored Password=") + wifiPassword);
    Serial.println(String("Stored Calibration=") + calibrationValue);
  }
}

void initDHTSensors() {
  dht1.begin();
  dht2.begin();
}

void initScale() {

  if(debug) Serial.println("Initializing HX711...");

  LoadCell.begin();
  unsigned long stabilizingtime = 2000; 
  bool _tare = true;
  LoadCell.start(stabilizingtime, _tare);

  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    if(debug) {
      Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    }
    while (1); // Halt
  } else {
    LoadCell.setCalFactor(calibrationValue);
    if(debug) {
      Serial.println("HX711 initialization complete. Calibration factor set.");
    }
  }
}

void connectToWiFi() {
  // Decide which SSID/PASS to use
  
  if (wifiSSID != "" && wifiPassword != "") {
    if(debug) {
      Serial.println("Using Stored Credentials...");
    }
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  } else {
    if(debug) {
      Serial.println("Using Secrets from thingProperties.h ...");
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin("FazioIOT", "824656789NAF");
    delay(100);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected IP Address: ");
      Serial.println(WiFi.localIP());
    }
  }

  // Attempt to connect for up to 10 seconds
  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000; // 10 seconds

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    if(debug) {
      Serial.print(".");
    }
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    if(debug) {
      Serial.println("\nConnected successfully.");
    }
  } else {
    if(debug) {
      Serial.println("\nFailed to connect. Will create Access Point if needed.");
    }
  }
}

void createAccessPointIfNeeded() {
  // If unable to connect, create access point
  if (WiFi.status() != WL_CONNECTED) {
    if(debug) {
      Serial.println("\nCreating Access Point...");
    }
    WiFi.mode(WIFI_AP);
    WiFi.softAP("BeehiveMonitor", "password1234");
    if(debug) {
      Serial.print("Access Point IP Address: ");
      Serial.println(WiFi.softAPIP());
    }
  }
}

///////////////////////////////////////////////
//           SENSOR/ SCALE READINGS
///////////////////////////////////////////////

void readDHTSensors() {
  h1 = dht1.readHumidity();
  t1 = dht1.readTemperature();
  h2 = dht2.readHumidity();
  t2 = dht2.readTemperature();

  // Update cloud variables in Fahrenheit (as used in your code)
  temp1 = (t1 * 9.0f / 5.0f) + 32.0f;
  temp2 = (t2 * 9.0f / 5.0f) + 32.0f;

  // Debug printing
  if(debug) {
    if(t1 >= 0) {
      Serial.println(String("Temp1: ") + temp1 + " || Humidity1: " + h1);
    }
    if(t2 >= 0) {
      Serial.println(String("Temp2: ") + temp2 + " || Humidity2: " + h2);
    }
  }
}

void measureBattery() {
  // Monitor the voltage divider on GPIO 32
  voltageDividerReading = 
      (analogRead(VOLTAGE_PIN) / 4095.0f) * 3.3f * voltageCalibrationFactor + voltageOffset;
  battery = voltageDividerReading; // IoT Cloud variable
  
  if(debug) {
    Serial.print("Voltage Divider Reading: ");
    Serial.println(voltageDividerReading);
  }
}

void updateScale() {
  static bool newDataReady = false;
  // Update the scale data
  if (LoadCell.update()) {
    newDataReady = true;
  }

  if (newDataReady) {
    float i = LoadCell.getData();
    if(debug) {
      Serial.print("Load_cell output RAW: ");
      Serial.println(i);
    }
    newDataReady = false;

    if (i <= 0) {
      i = 0;
    }
    last_weight = last_weightstore + i;

    grams  = last_weight;
    mVA    = movingAverage(last_weight);
    if(last_weight > 0) {
      weight = (float)last_weight / 28.35f; // convert to ounces
    } else {
      weight = 0.0f;
    }

    if(debug) {
      Serial.println(String("####Grams: ") + grams);
      Serial.println(String("####gMVA: ") + mVA);
      Serial.println(String("####Oz: ") + weight);
    }
    
    // Avoid negative weight
    if (last_weight <= 0) {
      last_weight = 0;
    }
  }
}

///////////////////////////////////////////////
//           TARE & CALIBRATION
///////////////////////////////////////////////

/**
 * movingAverage - calculates the running average of the latest NUM_SAMPLES
 * readings using a ring buffer.
 */
float movingAverage(int newValue) {
  static float readings[NUM_SAMPLES] = {0.0};
  static int   index = 0;
  static float sum   = 0;


  // Subtract the oldest reading from sum
  sum -= readings[index];

  // Store the new reading
  readings[index] = newValue;
  sum += newValue;

  // Advance the index, wrapping around
  index = (index + 1) % NUM_SAMPLES;

  // Return the average
  return sum / NUM_SAMPLES;
}

void tareScale() {
  // Start the tare process
  LoadCell.tareNoDelay();
  if(debug) {
    Serial.println("Tare started...");
  }

  // Reset variables after taring
  grams  = 0;
  mVA    = 0;
  weight = 0;


  if(debug) {
    Serial.println("Tare completed:");
    Serial.println("Raw Grams: 0");
    Serial.println("Moving Avg Grams: 0");
    Serial.println("Weight (oz): 0");
  }

  // Save reset values to Preferences
  prefs.begin("beehive-data");
  prefs.putInt("lastWeight", 0);
  prefs.end();

  if(debug) {
    Serial.println("Rebooting after tare...");
  }
  delay(500); // Allow Serial message to complete
  ESP.restart(); // Reboot the ESP
}
void recalibrateScale(float knownWeight) {
  if(debug) {
    Serial.println("Recalibration started...");
    Serial.println("Scale tared.");
    Serial.println("Place the known weight on the scale.");
  }

  // 1. Tare the scale
  LoadCell.tareNoDelay();
  delay(5000); // Wait for user to place weight

  // 2. Read the raw value
  float rawValue = LoadCell.getData();
  if(debug) {
    Serial.print("Raw value read from HX711: ");
    Serial.println(rawValue);
  }

  // 3. Calculate new calibration factor
  float newCalFactor = rawValue / knownWeight;
  if(debug) {
    Serial.print("New Calibration Factor: ");
    Serial.println(newCalFactor);
  }

  // 4. Update the calibration factor
  LoadCell.setCalFactor(newCalFactor);

  // 5. Store the calibration factor
  prefs.begin("beehive-data");
  prefs.putFloat("calibrationFactor", newCalFactor);
  float check = prefs.getFloat("calibrationFactor", NAN);
  if(debug) {
    Serial.println(String("CHECK=  ") + check);
  }
  prefs.end();

  if(debug) {
    Serial.println("Calibration factor updated and saved.");
    Serial.println("Remove the weight.....");
  }
  delay(3000);
}
///////////////////////////////////////////////
//              UTILITY
///////////////////////////////////////////////
void handleSerialCommands() {
  if(debug && Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command.startsWith("CAL")) {
      float knownWeight = command.substring(3).toFloat();
      recalibrateScale(knownWeight);
    }
    else if (command.startsWith("SET")) {
      calibrationValue = command.substring(3).toFloat();
      LoadCell.setCalFactor(calibrationValue);
      Serial.println("Cal Set");
    }
    else if (command.startsWith("TART")) {
      tareScale();
unsigned long lastWifiCheckTime = 0;
  }
 }
}

float lastWifiCheckTime = 0;
void checkforWifi(){
  if (millis() - lastWifiCheckTime < 60000) {
    return;
  }
  if (WiFi.status() == WL_CONNECTED) {
    if(debug) {
      Serial.println("Connected to WiFi.");
    }
  } else {
    if(debug) {
      Serial.println("Not connected to WiFi. Restarting");
      ESP.restart();

    }
  }
}


void enterDeepSleep() {
  if(debug) {
    Serial.println("Entering deep sleep for 30 minutes...");
  }
  // Configure the wake-up source as a timer
  esp_sleep_enable_timer_wakeup(30 * 60 * 1000000); // 30 minutes in microseconds

  // Enter deep sleep mode
  esp_deep_sleep_start();
}

