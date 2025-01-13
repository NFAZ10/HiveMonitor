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
  connectToWiFi();
  createAccessPointIfNeeded();
  setupWebServer(); // Ensure this function is properly defined in webserver.h and webserver.cpp
  if (WiFi.status() == WL_CONNECTED) {
    initAWS();
    connectToAWS();
  }

  Serial.println("Setup Complete");
}


unsigned long lastPublishTime = 0; // Initialize outside the loop
const unsigned long publishInterval = 30000; // 15 minutes in milliseconds
int awsConnectAttempts = 0;
void loop() {
  checkForUpdates();
  if (!mqttClient.connected()) {
    connectToAWS();
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

  // If WiFi is connected and AWS is connected, publish data to AWS IoT
  if (WiFi.status() == WL_CONNECTED && mqttClient.connected()) {
      initAWS();
      awsConnectAttempts++;
      Serial.println(String("Attempts: ") + awsConnectAttempts);
      if (awsConnectAttempts >= 5) {
        Serial.println("Failed to connect to AWS IoT after 5 attempts. Rebooting...");
        ESP.restart();
      }
    } else {
      awsConnectAttempts = 0; // Reset the counter if connected
    }
    mqttClient.loop();

    // Publish data to AWS IoT
    DynamicJsonDocument doc(256);
    doc["temperature1"] = t1;
    doc["humidity1"] = h1;
    doc["temperature2"] = t2;
    doc["humidity2"] = h2;
    doc["weight"] = grams;
    doc["battery"] = voltageDividerReading;
    doc["version"] = currentVersion;

    String jsonData;

   while(millis() - lastPublishTime <= 10000) {
      serializeJson(doc, jsonData);
      uint8_t mac[6];
      WiFi.macAddress(mac);
      char macStr[13];
      snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
      String topic = "beehive/data/";
      Serial.println(macStr);
      topic = String(topic+macStr); // Get the last 4 digits of the MAC address
      mqttClient.publish(topic.c_str(), jsonData.c_str());
      lastPublishTime = millis(); // Update the last publish time
      Serial.println(topic);
      if(debug) {
        Serial.println("Published data to AWS IoT: ");
        Serial.println(jsonData);
      }
    }
  


  delay(1000);
}