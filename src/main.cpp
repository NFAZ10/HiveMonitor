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
  initMQTT();
  connectToWiFi();
  createAccessPointIfNeeded();
  setupWebServer(); // Ensure this function is properly defined in webserver.h and webserver.cpp
  if (WiFi.status() == WL_CONNECTED) {

    connectToMQTT();
  }

  Serial.println("Setup Complete");
}



void loop() {
  checkForUpdates();
  if (!mqttClient.connected()&&WiFi.status() == WL_CONNECTED) {
    connectToMQTT();
  }

  if (WiFi.status() == WL_CONNECTED) {
  mqttClient.loop();
  }
 
  
  if(debug) {
    Serial.println("##################################");
  }

  handleSerialCommands();
  measureBattery();

  readDHTSensors();

  // Check if tare is requested via IoT or locally
  if (tareRequested) {
    tareScale();
    // Clear the IoT Cloud variable after taring
    tareRequested = false;
  }

   if (WiFi.status() == WL_CONNECTED) {
    mqttClient.loop();

    if (millis() - lastPublishTime > 10000) {
      uint8_t mac[6];
      WiFi.macAddress(mac);
      char macStr[13];
      snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
      String topicBase = "beehive/data/";
      topicBase += macStr; // Get the last 4 digits of the MAC address
      Serial.println("Sending data to topic: " + topicBase + "/temperature1");
      mqttClient.publish((topicBase + "/temperature1").c_str(), String(t1).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/humidity1");
      mqttClient.publish((topicBase + "/humidity1").c_str(), String(h1).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/temperature2");
      mqttClient.publish((topicBase + "/temperature2").c_str(), String(t2).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/humidity2");
      mqttClient.publish((topicBase + "/humidity2").c_str(), String(h2).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/weight");
      mqttClient.publish((topicBase + "/weight").c_str(), String(grams).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/battery");
      mqttClient.publish((topicBase + "/battery").c_str(), String(voltageDividerReading).c_str());
      delay(1000);
      Serial.println("Sending data to topic: " + topicBase + "/version");
      mqttClient.publish((topicBase + "/version").c_str(), String(currentVersion).c_str());
      delay(1000);


      lastPublishTime = 0; // Update the last publish time
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
      }
    }
  }

Serial.println(String("Last publish: ") + lastPublishTime);
  delay(1000);

checkforWifi();
  if(voltageDividerReading < 3.3){
    enterDeepSleep();
  }
  else if (voltageDividerReading > 3.3){
    if(debug) {
      Serial.println("Battery is above 3.3V");
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

