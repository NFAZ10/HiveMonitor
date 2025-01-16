#include <WiFi.h>
#include <PubSubClient.h>
#include "globals.h"
#include "webserver.h"
#include "aws.h"
#include "functions.h"
#include "ota.h"
#include "var.h"


void setup() {
  initSerial();
  initPreferences();
  loadPreferences();
  initDHTSensors();
  initScale();
  //clearPreferences();
  // tareScale(); 
  connectToWiFi();
  createAccessPointIfNeeded();
  setupWebServer(); // Ensure this function is properly defined in webserver.h and webserver.cpp

  // Only initialize and connect to the local MQTT server if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    mqttClient.setServer(mqttServer, mqttPort);
    mqttClient.setCallback(messageHandler); // Set the callback function
    connectToMQTT();
  } else {
    Serial.println("WiFi not connected, skipping MQTT setup");
  }
  createAccessPointIfNeeded();
  setupWebServer();




  Serial.println("Setup Complete");
}

void loop() {
  

  if(debug) {
    Serial.println("##################################");
  }

 handleSerialCommands();
 measureBattery();
 readDHTSensors();
 checkForUpdates();
 updateScale();

 Serial.println(String("Last MVA:  ")+mVA);
 Serial.println(String("Last Weight:  ")+last_weightstore);

if (last_weightstore-grams>=10000||grams<0){
  Serial.println("Weight has changed by 10kg, updating last weight");
  prefs.begin("beehive",false);
  prefs.putInt("Weight", grams);
  prefs.putFloat("mVA", grams);
  prefs.end();
  loadPreferences();
  
}

 weight = grams;

 Serial.println(String("Updated MVA:  ")+mVA);
 Serial.println(String("Updated Weight:  ")+weight);

prefs.begin("beehive",false);
prefs.putInt("Weight", grams);
prefs.putFloat("mVA", mVA);

int weighttest= prefs.getInt("Weight", 0);
Serial.println(String("Weight Test:  ")+weight);
prefs.end();  

 


  if (WiFi.status() == WL_CONNECTED) {
      if (!mqttClient.connected()) {
    connectToMQTT();
  }
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

      lastPublishTime = millis(); // Update the last publish time
      Serial.println(topicBase);
      if (debug) {
        Serial.println("Published data to MQTT:");
        Serial.println("temperature1: " + String(t1));
        Serial.println("humidity1: " + String(h1));
        Serial.println("temperature2: " + String(t2));
        Serial.println("humidity2: " + String(h2));
        Serial.println("weight: " + String(grams));
        Serial.println("battery: " + String(voltageDividerReading));
        Serial.println("version: " + String(currentVersion));
        Serial.println("mva: " + String(mVA));
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
      enterNap();
    }
  }
  else if (voltageDividerReading>4.1){
        if(debug) {
      Serial.println("Battery is Fully Charged");
    }
  }
  {
    /* code */
  }
  


  delay(1000);
}

