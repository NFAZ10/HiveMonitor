#include "globals.h"
#include <Arduino.h>
#include "var.h"
#include "secrets.h"
#include "functions.h"


// Function to perform OTA
void performOTA() {
  HTTPClient http;
  http.begin(firmwareURL);

  int httpCode = http.GET();
  if (httpCode == 200) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      size_t written = Update.writeStream(*stream);

      if (written == contentLength) {
        Serial.println("Firmware written successfully. Rebooting...");
        if (Update.end()) {
          if (Update.isFinished()) {
            Serial.println("Update successful. Rebooting...");
            ESP.restart();
          } else {
            Serial.println("Update not finished. Something went wrong.");
          }
        } else {
          Serial.println("Update failed. Error #: " + String(Update.getError()));
        }
      } else {
        Serial.println("Written only " + String(written) + "/" + String(contentLength) + ". Retry?");
      }
    } else {
      Serial.println("Not enough space for OTA update.");
    }
  } else {
    Serial.println("Failed to download firmware. HTTP Code: " + String(httpCode));
  }
  http.end();
}

// Function to check for updates
void checkForUpdates() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Get the latest version from GitHub
    http.begin(versionURL);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String newVersion = http.getString();
      newVersion.trim();

      Serial.println("Current Version: " + currentVersion);
      Serial.println("Latest Version: " + newVersion);

      // Compare versions
      if (newVersion != currentVersion) {
        Serial.println("New firmware available. Starting OTA update...");
        performOTA();
      } else {
        Serial.println("Device is already up to date.");
      }
    } else {
      Serial.println("Failed to fetch version file. HTTP Code: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
