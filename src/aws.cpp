#include <Arduino.h>
#include "globals.h"
#include "aws.h"
#include "var.h"


void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println("Message: " + message);
  // Add your logic to handle the received message
if (message.startsWith("setscale(") && message.endsWith(")")) {
  String numberStr = message.substring(9, message.length() - 1);
  float scaleValue = numberStr.toFloat();
  
  prefs.begin("beehive", false);
  prefs.putFloat("Weight", scaleValue);
  prefs.end();
  
  Serial.println("Scale set to: " + String(scaleValue));
  Serial.println("Rebooting...");
  ESP.restart();
}
}

void initMQTT() {
  // Configure the MQTT broker
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.println("MQTT Client Initialized.");
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP32Client-" + String(WiFi.macAddress());
    if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Subscribe to the tare topic
      String topicBase = "beehive/data/" + String(WiFi.macAddress()) + "/cmd";
      mqttClient.subscribe(topicBase.c_str());
      Serial.println("Subscribed to topic: " + topicBase);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
