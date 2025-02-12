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
  
}

void loadPreferences() {
  prefs.begin("beehive"),false;
  last_weightstore = prefs.getInt("Weight");
  mVA              = prefs.getFloat("mVA");
  wifiSSID         = prefs.getString("wifiSSID", "");
  wifiPassword     = prefs.getString("wifiPassword", "");
  calibrationValue = prefs.getFloat("calibrationFactor", CALIBRATION_FACTOR);

  prefs.end();


  if(debug) {
    Serial.println(String("Last Weight=") + last_weightstore);
    Serial.println(String("Last mVA=") + mVA);
    Serial.println(String("Stored SSID=") + wifiSSID);
    Serial.println(String("Stored Password=") + wifiPassword);
    Serial.println(String("Stored Calibration=") + calibrationValue);
  }
}

void clearPreferences() {
  prefs.begin("beehive",false);
  prefs.putInt("Weight",0);
  prefs.putFloat("mVA",0.0);
  last_weightstore = prefs.getInt("Weight");
  mVA              = prefs.getFloat("mVA");
  Serial.println(String("Last Weight=") + last_weightstore);
  Serial.println(String("Last mVA=") + mVA);
  Serial.println("Preferences Cleared");
  prefs.end();
}

void initDHTSensors() {
  dht1.begin();
  dht2.begin();
}

void initScale() {

  if(debug) Serial.println("Initializing HX711...");
  LoadCell.begin();
  
  LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive


  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    if(mVA>0){
      if(mVA-last_weightstore>=10000){
        Serial.println("Negative Weight Detected. Taring...");
        tareScale();
      }
      mVA = mVA*-calibrationValue;
     
    }
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)

    
    Serial.println("Startup is complete");
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
      strip.setPixelColor(0,0,255,255); //  Set pixel's color (in RAM)
      strip.show();
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
  // Check for NaN values and set to null if found
  if (isnan(h1)) h1 = -1;
  if (isnan(t1)) t1 = -1;
  if (isnan(h2)) h2 = -1;
  if (isnan(t2)) t2 = -1;
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

  // Adjust the calibration factor to match the actual voltage
  voltageDividerReading *= (4.2 / 4.28);
  battery = voltageDividerReading; // IoT Cloud variable
  
  if(debug) {
    Serial.print("Voltage Divider Reading: ");
    Serial.println(voltageDividerReading);
  }
}


void updateScale() {
  static bool newDataReady = false;
  int sampleCount = 10;
  int total = 0 ;
if(debug){
Serial.println("Reading Scale");
      strip.setPixelColor(0,255,255,15); //  Set pixel's color (in RAM)
      strip.show();


}
 
   for (int i = 0; i < sampleCount; i++) {
    while (!LoadCell.update()) {
      //Serial.print("Reading Scale:  ");
      //Serial.println(LoadCell.getData());
      Serial.print(".");
    }
    total += LoadCell.getData();
   // Serial.println(String("Raw Data: ") + LoadCell.getData()*calibrationValue);

    
  }
  
       Serial.print(String("Total: ") + total); Serial.println(String(" || SampleCount: ") + sampleCount);
      
      grams= total/sampleCount;
      Serial.println(String("Last Weight: ") + last_weightstore);
      grams=grams+last_weightstore; //set offset from last weight

      if(grams < 0){
        Serial.println("Negative Weight Detected. Taring...");
        grams = 0;
       
        tareScale();
      }
   
      //grams=LoadCell.getData();

  mVA= movingAverage(grams);
  Serial.println(String("Grams: ") + grams);
  // Convert grams to pounds (1 gram = 0.00220462 pounds)
  weightInPounds = grams * 0.00220462;
  Serial.println(String("Weight in Pounds: ") + weightInPounds);
    
      strip.setPixelColor(0,0,0,0); //  Set pixel's color (in RAM)
      strip.show();
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

  LoadCell.update();

   LoadCell.refreshDataSet();
   LoadCell.resetSamplesIndex();
   //LoadCell.setTareOffset(0);


  LoadCell.tareNoDelay();
  if(debug) {
    Serial.println("Tare started...");
  }

  // Reset variables after taring
  grams  = 0;
  mVA    = 0;
  weight = 0;
  prefs.begin("beehive-data",false);
  prefs.putInt("lastWeight", grams);
  prefs.putInt("Weight", grams);
  prefs.putFloat("mVA", mVA);
  prefs.end();



  if(debug) {
    Serial.println("Tare completed:");
    Serial.println("Raw Grams: 0");
    Serial.println("Moving Avg Grams: 0");
    Serial.println("Weight (oz): 0");
  }



  if(debug) {
    Serial.println("Rebooting after tare...");
  }
  delay(500); // Allow Serial message to complete
  weightset=false;
  //ESP.restart(); // Reboot the ESP
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
    else if (command.startsWith("NEW")) {
       clearPreferences();
       Serial.println("Preferences Cleared");
    }
    else if (command.startsWith("TART")) {
      tareScale();
    }
    else if(command.startsWith("WEIGHT")){
      weightset=true;
      last_weightstore = command.substring(6).toInt();
      Serial.println("Weight Set");
    }
    else if(command.startsWith("DEBUG")){
      debug = !debug;
      Serial.println("Debug Set");
    }
    else if(command.startsWith("REBOOT")){
      ESP.restart();
    }
    else if(command.startsWith("SLEEP")){
      enterDeepSleep();
    }
    else if(command.startsWith("NAP")){
      enterNap();
    }
    else if(command.startsWith("WIFI")){
      connectToWiFi();
    }
    else if(command.startsWith("AP")){
      createAccessPointIfNeeded();
    }
    else if(command.startsWith("OFF")){
      turnOffWiFi();
    }
    else if(command.startsWith("PREF")){
      loadPreferences();
    }
    else if(command.startsWith("CLEAR")){
      clearPreferences();
    }
    else if(command.startsWith("READ")){
      readDHTSensors();
    }
    else if(command.startsWith("SCALE")){
      updateScale();
    }
    else if(command.startsWith("BATTERY")){
      measureBattery();
    }
    else if(command.startsWith("TARE")){
      tareScale();
    }
    else if(command.startsWith("RECAL")){
      recalibrateScale(1000);
    }
    else if(command.startsWith("MVA")){
      movingAverage(grams);
    }
    else if(command.startsWith("SERIAL")){
      Serial.println("Serial Commands");
    }
    else {
      Serial.println("Unknown command");
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
    Serial.println("Entering deep sleep for 30  minutes...");
  }
  // Convert time from minutes to microseconds
  int timeInMicroseconds = 30 * 60 * 1000000;
  // Configure the wake-up source as a timer
  esp_sleep_enable_timer_wakeup(timeInMicroseconds);
  delay(1000);
  // Enter deep sleep mode
  esp_deep_sleep_start();
  delay(1000);
}

void enterNap() {
  if(debug) {
    Serial.println("Entering deep sleep for 1 minutes...");
  }
  // Convert time from minutes to microseconds
  int timeInMicroseconds = 1 * 60 * 1000000;
  // Configure the wake-up source as a timer
  esp_sleep_enable_timer_wakeup(timeInMicroseconds);
  delay(1000);
  // Enter deep sleep mode
  esp_deep_sleep_start();
  delay(1000);
}

void turnOffWiFi() {
  if (debug) {
    Serial.println("Turning off WiFi...");
  }
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop(); // Turn off Bluetooth if it's enabled
}
