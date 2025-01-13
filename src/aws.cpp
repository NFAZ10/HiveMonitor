#include <Arduino.h>
#include "secrets.h"
#include "globals.h"
#include "aws.h"

void initAWS() {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  secureClient.setCACert(AWS_CERT_CA);
  secureClient.setCertificate(AWS_CERT_CRT);
  secureClient.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Configure the MQTT broker
  mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
  Serial.println("AWS IoT Client Initialized.");
}

void connectToAWS() {
  if (WiFi.status() == WL_CONNECTED) {
  Preferences prefs;
  prefs.begin("device-data", false);

  // Retrieve or generate the Thing Name
  String thingName = prefs.getString("thingName", "");
  if (thingName == "") {
    thingName = "Device-" + String(WiFi.macAddress());
    prefs.putString("thingName", thingName);
  }
  prefs.end();

  Serial.println("Using Thing Name: " + thingName);

  // Connect to the MQTT broker
  Serial.println("Connecting to AWS IoT...");
  while (!mqttClient.connected()) {
    if (mqttClient.connect(thingName.c_str())) {
      Serial.println("AWS IoT Connected!");
    } else {
      Serial.print("Failed to connect. MQTT State: ");

      Serial.println(mqttClient.state());
      delay(2000);
    }
   }
  

WiFi.setHostname(thingName.c_str());
  // Subscribe to AWS IoT topiczzx
  mqttClient.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("Subscribed to topic: " + String(AWS_IOT_SUBSCRIBE_TOPIC));
}
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println("Message: " + message);
  // Add your logic to handle the received message
}
