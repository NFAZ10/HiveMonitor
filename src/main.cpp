#include <WiFi.h>
#include <PubSubClient.h>
#include "globals.h"
#include "webserver.h"
#include "aws.h"
#include "functions.h"
#include "ota.h"
#include "var.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_SLAVE_ADDR 0x42  


float dbLevel = 0;
float dominantFreq = 0;

void receiveData(int byteCount) {
  WebSerial.println("Received data from I2C");
    if (byteCount == sizeof(float) * 2) {
        Wire.readBytes((byte*)&dbLevel, sizeof(dbLevel));  
        Wire.readBytes((byte*)&dominantFreq, sizeof(dominantFreq));
    }
}




void setup() {
    initSerial();
    initPreferences();
    loadPreferences();
    initDHTSensors();
    initScale();
    
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.onReceive(receiveData);
     
    // clearPreferences();  // Uncomment if you want to clear stored preferences
    // tareScale();         // Uncomment if you want to tare the scale on startup
    connectToWiFi();
    createAccessPointIfNeeded(); // Call once to handle access point creation

   
    //Initialize NeoPixel strip
    strip.begin();
    strip.show(); // Turn OFF all pixels ASAP
    strip.setBrightness(50);

    // Initialize MQTT if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        mqttClient.setServer(mqttServer, mqttPort);
        mqttClient.setCallback(messageHandler);
        connectToMQTT();
    } else {
        Serial.println("WiFi not connected, skipping MQTT setup");
    }

    // Set up the web server
   setupWebServer();
   Serial.println(WiFi.localIP());

   Serial.print("Heap before FFT: ");
   Serial.println(esp_get_free_heap_size());
  
}

void loop() {
  

  if(debug) {
    Serial.println("##################################");
  }
  // Pause to ensure no serial or web commands were entered
  delay(3000); // Adjust the delay as needed
 handleSerialCommands();
 measureBattery();
 readDHTSensors();
 checkForUpdates();
 updateScale();



 WebSerial.println(String("Last MVA:  ")+mVA);
 WebSerial.println(String("Last Weight:  ")+last_weightstore);

if (last_weightstore-grams>=10000||grams<0){
    strip.setPixelColor(0,255,0,0); //  Set pixel's color (in RAM)
    strip.show();
  Serial.println("Weight has changed by 10kg, updating last weight");
  prefs.begin("beehive",false);
  prefs.putInt("Weight", grams);
  prefs.putFloat("mVA", grams);
  prefs.end();
  loadPreferences();
  
}

 weight = grams;

 WebSerial.println(String("Updated MVA:  ")+mVA);
 WebSerial.println(String("Updated Weight:  ")+weight);

prefs.begin("beehive",false);
prefs.putInt("Weight", grams);
int storedweight = prefs.getInt("Weight", grams);
prefs.putFloat("mVA", mVA);
Serial.println(String("# Stored Weight:  ")+storedweight);

int weighttest= prefs.getInt("Weight", 0);
WebSerial.println(String("Weight Test:  ")+weight);
prefs.end();  

 


  if (WiFi.status() == WL_CONNECTED) {
      if (!mqttClient.connected()) {
    connectToMQTT();
  }
  strip.setPixelColor(0,0,255,0); //  Set pixel's color (in RAM)
  strip.show();
    mqttClient.loop();

    if (millis() - lastPublishTime >= publishInterval) {
      uint8_t mac[6];
      WiFi.macAddress(mac);
      char macStr[13];
      snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
      String topicBase = "beehive/data/";
      topicBase += macStr; // Get the last 4 digits of the MAC address

      mqttClient.publish((topicBase + "/temperature1").c_str(), String(t1).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/humidity1").c_str(), String(h1).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/temperature2").c_str(), String(t2).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/humidity2").c_str(), String(h2).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/weight").c_str(), String(grams).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/battery").c_str(), String(voltageDividerReading).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/version").c_str(), String(currentVersion).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/mva").c_str(), String(mVA).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/lbs").c_str(), String(weightInPounds).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/db").c_str(), String(dbLevel).c_str());
      delay(1000);
      mqttClient.publish((topicBase + "/Hz").c_str(), String(dominantFreq).c_str());
      delay(1000);

      lastPublishTime = millis(); // Update the last publish time
      Serial.println(topicBase);
      if (debug) {
        WebSerial.println("Published data to MQTT:");
        WebSerial.println("temperature1: " + String(t1));
        WebSerial.println("humidity1: " + String(h1));
        WebSerial.println("temperature2: " + String(t2));
        WebSerial.println("humidity2: " + String(h2));
        WebSerial.println("weight: " + String(grams));
        WebSerial.println("battery: " + String(voltageDividerReading));
        WebSerial.println("version: " + String(currentVersion));
        WebSerial.println("mva: " + String(mVA));
        WebSerial.println("lbs: " + String(weightInPounds));
        WebSerial.println("db: " + String(dbLevel));
        WebSerial.println("Hz: " + String(dominantFreq));

      }
    }
  }

  // Check if tare is requested via IoT or locally
  if (tareRequested) {
    tareScale();
    // Clear the IoT Cloud variable after taring
    tareRequested = false;
  }




  Serial.println(String("Last publish: ") + lastPublishTime);
  delay(1000);

checkforWifi();
  if(voltageDividerReading < 3.3&& voltageDividerReading != 0){
    enterDeepSleep();
  }
  else if (voltageDividerReading > 3.3&& voltageDividerReading < 3.9){
    if(debug) {
      Serial.println("Battery is above 3.3V");
      strip.setPixelColor(0,100,100,0); //  Set pixel's color (in RAM)
      strip.show();
      delay(1000);
      strip.setPixelColor(0,0,0,0); //  Set pixel's color (in RAM)
      strip.show();
      enterNap();
    }
  }
  else if (voltageDividerReading>4.1){
        if(debug) {
      Serial.println("Battery is Fully Charged");
    }

      strip.setPixelColor(0,100,0,100); //  Set pixel's color (in RAM)
      strip.show();
      delay(1000);
      strip.setPixelColor(0,0,0,0); //  Set pixel's color (in RAM)
      strip.show();
  }
  {
    /* code */
  }
  prefs.begin("beehive",false);
  prefs.putFloat("calibrationFactor", calibrationValue);
  prefs.end();


  delay(1000);
}



