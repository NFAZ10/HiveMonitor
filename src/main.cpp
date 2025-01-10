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
  setupWebServer();
  initAWS();
  connectToAWS();
  Serial.println("Setup Complete");
}

unsigned long lastPublishTime = 0; // Initialize outside the loop
const unsigned long publishInterval = 15 * 60 * 1000; // 15 minutes in milliseconds
int awsConnectAttempts;
void loop() {
  checkForUpdates();
  if (!mqttClient.connected()) {
    connectToAWS();
  }
  mqttClient.loop();
  
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

  // If WiFi is connected, publish data to AWS IoT
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqttClient.connected()) {
      initAWS();
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

    if (millis() - lastPublishTime >= publishInterval) {
      serializeJson(doc, jsonData);
      uint8_t mac[6];
      WiFi.macAddress(mac);
      char macStr[13];
      static int awsConnectAttempts = 0;
      snprintf(macStr, sizeof(macStr), "%02X%02X%02X%02X", mac[2], mac[3], mac[4], mac[5]);
      String topic = "beehive/data/";
      topic += String(macStr).substring(8); // Get the last 4 digits of the MAC address
      mqttClient.publish(topic.c_str(), jsonData.c_str());
      lastPublishTime = millis(); // Update the last publish time
      if(debug) {
        Serial.println("Published data to AWS IoT: ");
        Serial.println(jsonData);
      }
    }

    if (!mqttClient.connected()) {
      awsConnectAttempts++;
      if (awsConnectAttempts >= 5) {
        Serial.println("Failed to connect to AWS IoT after 5 attempts. Rebooting...");
        ESP.restart();
      }
    } else {
      awsConnectAttempts = 0; // Reset the counter if connected
    }
  }

  delay(1000); // Add a delay to prevent the loop from running too fast
  // Add functionality here if needed
}
