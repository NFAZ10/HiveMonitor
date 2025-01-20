#include "globals.h"
#include <ArduinoJson.h>
#include <Arduino.h>
#include "webserver.h"
#include "functions.h"
#include "var.h"
#include <WebSerial.h>
AsyncWebServer server(80);
void recvMsg(uint8_t *data, size_t len) {
    WebSerial.println("Received Data...");

    String msg = "";  // Initialize an empty string
    for (int i = 0; i < len; i++) {
        msg += (char)data[i];  // Append each character to the string
    }

    WebSerial.println(msg);  // Send the reconstructed string to WebSerial
}
///////////////////////////////////////////////
//            WEB SERVER SETUP
///////////////////////////////////////////////
void setupWebServer() {
  Serial.println("Setting up Web Server...");
  // Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; }";
    html += "h1 { color: #333; }";
    html += "button { padding: 10px 20px; margin: 10px; font-size: 16px; }";
    html += "table { margin: 20px auto; border-collapse: collapse; width: 80%; }";
    html += "table, th, td { border: 1px solid #333; padding: 10px; }";
    html += "th { background-color: #333; color: white; }";
    html += "</style></head><body><h1>Beehive Monitor</h1>";
    html += "<p>Weight: " + String(grams) + " g</p>";
    html += "<p>Temperature 1: " + String(t1) + " &#8451;</p>";
    html += "<p>Humidity 1: " + String(h1) + " %</p>";
    html += "<p>Temperature 2: " + String(t2) + " &#8451;</p>";
    html += "<p>Humidity 2: " + String(h2) + " %</p>";
    html += "<p>Calibration Status: " + calibrationStatus + "</p>";
    html += "<p>MAC Address: " + WiFi.macAddress() + "</p>";
    html += "<h1>OTA Update</h1>";
    html += "<form action=\"/update\" method=\"POST\">";
    html += "<label for=\"branch\">Select Branch:</label>";
    html += "<select name=\"branch\" id=\"branch\">";
    html += "<option value=\"main\">Main</option>";
    html += "<option value=\"dev\">Dev</option>";
    html += "</select>";
    html += "<input type=\"submit\" value=\"Update\">";
    html += "</form>";
    html += "<button onclick=\"fetch('/tare', {method: 'POST'}).then(() => alert('Tared!'))\">Tare Scale</button>";
    html += "<button onclick=\"fetch('/lowpower', {method: 'POST'}).then(() => alert('Low Power Mode Toggled'))\">Toggle Low Power Mode</button>";
    html += "<button onclick=\"fetch('/reboot', {method: 'POST'}).then(() => alert('Rebooting...'))\">Restart Device</button>";
    html += "<a href='/data'>View Historical Data</a>";
    html += "<h3>Change Wi-Fi Settings</h3>";
    html += "<form onsubmit=\"submitWifiForm(event)\">";
    html += "SSID: <input type='text' id='ssid' name='ssid'><br><br>";
    html += "Password: <input type='password' id='password' name='password'><br><br>";
    html += "<input type='submit' value='Save Wi-Fi Settings'>";
    html += "</form>";
    html += "<script>";
    html += "function submitWifiForm(event) {";
    html += "  event.preventDefault();";
    html += "  let ssid = document.getElementById('ssid').value;";
    html += "  let password = document.getElementById('password').value;";
    html += "  fetch('/wifi', {method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ ssid: ssid, password: password })})";
    html += "    .then(() => alert('Wi-Fi Settings Updated'));";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });
  // Endpoint to show serial terminal
  server.on("/terminal", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; }";
    html += "h1 { color: #333; }";
    html += "textarea { width: 80%; height: 300px; }";
    html += "</style></head><body><h1>Serial Terminal</h1>";
    html += "<textarea id='terminal' readonly></textarea><br>";
    html += "<input type='text' id='input' placeholder='Type a command'><button onclick='sendCommand()'>Send</button>";
    html += "<script>";
    html += "function sendCommand() {";
    html += "  let input = document.getElementById('input').value;";
    html += "  fetch('/send', {method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ command: input })})";
    html += "    .then(response => response.text())";
    html += "    .then(data => {";
    html += "      let terminal = document.getElementById('terminal');";
    html += "      terminal.value += '\\n' + data;";
    html += "      terminal.scrollTop = terminal.scrollHeight;";
    html += "    });";
    html += "}";
    html += "setInterval(() => {";
    html += "  fetch('/receive').then(response => response.text()).then(data => {";
    html += "    let terminal = document.getElementById('terminal');";
    html += "    terminal.value += '\\n' + data;";
    html += "    terminal.scrollTop = terminal.scrollHeight;";
    html += "  });";
    html += "}, 1000);";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });



 

  // Endpoint to receive data from WebSerial
//dpoint to tare the scale
  server.on("/tare", HTTP_POST, [](AsyncWebServerRequest *request) {
    tareRequested = true;
    request->send(200, "text/plain", "Tare requested");
  });
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("branch", false)) {
      otaBranch = request->getParam("branch", false)->value();
      if (otaBranch == "main") {
         prefs.begin("beehive-data", false);
         prefs.putString("branch", "main");
         prefs.end();
      } else if (otaBranch == "dev") {
         prefs.begin("beehive-data", false);
         prefs.putString("branch", "dev");
         prefs.end();
      }
      request->send(200, "text/html", "OTA update initiated for branch: " + otaBranch);
  
    } else {
      request->send(400, "text/html", "Branch not selected");
    }
  });
  // Endpoint to toggle low power mode
  server.on("/lowpower", HTTP_POST, [](AsyncWebServerRequest *request) {
    prefs.begin("beehive-data", false);
    lowPowerMode = !lowPowerMode;
    prefs.putBool("lowPowerMode", lowPowerMode);
    prefs.end();
    request->send(200, "text/plain", "Low power mode toggled");
  });

  // Endpoint to get historical data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body><h1>Historical Data</h1>";
    html += "<table border='1'><tr><th>Time</th><th>Weight</th><th>Temperature 1</th><th>Humidity 1</th><th>Temperature 2</th><th>Humidity 2</th></tr>";
    
    prefs.begin("beehive-data");
    for (int i = 0; i < MAX_DATA_POINTS; i++) {
      String key = "data" + String(i);
      String jsonData = prefs.getString(key.c_str(), "");
      if (jsonData != "") {
        StaticJsonDocument<256> doc;
        deserializeJson(doc, jsonData);
        html += "<tr><td>" + String(doc["time"].as<long>()) + "</td><td>" 
                + String(doc["weight"].as<float>()) + " kg</td><td>"
                + String(doc["temp1"].as<float>()) + " &#8451;</td><td>"
                + String(doc["humidity1"].as<float>()) + " %</td><td>"
                + String(doc["temp2"].as<float>()) + " &#8451;</td><td>"
                + String(doc["humidity2"].as<float>()) + " %</td></tr>";
      }
    }
    prefs.end();
    html += "</table></body></html>";
    request->send(200, "text/html", html);
  });

  // Endpoint to update Wi-Fi settings
  server.on("/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("body", true)) {
      String body = request->getParam("body", true)->value();
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, body);
      wifiSSID     = doc["ssid"].as<String>();
      wifiPassword = doc["password"].as<String>();
      Serial.println("Wi-Fi SSID: " + wifiSSID);
      Serial.println("Wi-Fi Password: " + wifiPassword);
      Serial.println("Updating Wi-Fi settings...");
      prefs.begin("beehive-data", false);
      prefs.putString("wifiSSID", wifiSSID);
      prefs.putString("wifiPassword", wifiPassword);
      prefs.end();

      request->send(200, "text/plain", "Wi-Fi settings updated. Reboot to apply changes.");
    } else {
      request->send(400, "text/plain", "Invalid request");
    }
  });

  // Endpoint to reboot the device
server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
  request->send(200, "text/plain", "Rebooting...");
  delay(1000);
  ESP.restart();
});
 WebSerial.begin(&server);
    WebSerial.onMessage(recvMsg);
    server.begin();

  Serial.println("Web Server Ready.");

}

