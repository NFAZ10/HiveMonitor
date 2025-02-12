#include <Arduino.h>
#include <SoftwareSerial.h>


#define RXD2 16  // ESP32 RX2
#define TXD2 17  // ESP32 TX2

HardwareSerial sim800(1);

void setupcell() {
    
    sim800.begin(115200, SERIAL_8N1, RXD2, TXD2);

    Serial.println("Initializing SIM800L...");
    delay(1000);
    sim800.println("AT");
}

void checkSIM(){
sim800.println("AT");
sim800.println("AT+CSQ");  // Signal quality
sim800.println("AT+CCID"); // SIM card detection
sim800.println("AT+COPS?"); // Network operator
sim800.println("AT+CGREG?"); // Network registration
}

void sendSMS(){
    sim800.println("AT+CMGF=1");  // SMS mode
delay(100);
sim800.println("AT+CMGS=\"+1234567890\"");  // Replace with phone number
delay(100);
sim800.print("Hello from ESP32!");
delay(100);
sim800.write(26);  // ASCII CTRL+Z to send
delay(100);
}

void setupGPRS() {
    sim800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
    delay(100);
    sim800.println("AT+SAPBR=3,1,\"APN\",\"your_apn_here\"");
    delay(100);
    sim800.println("AT+SAPBR=1,1");
    delay(1000);
    sim800.println("AT+SAPBR=2,1"); // Check if connected
    delay(100);
}

void CELLconnectToMQTT() {
    sim800.println("AT+MQTTCONN=0,\"broker.hivemq.com\",1883,60");
    delay(1000);
    sim800.println("AT+MQTTSUB=0,\"beehive/data\",0");  // Subscribe to a topic
}

void publishData(float weight, float temp1, float temp2, float humidity1, float humidity2) {
    String payload = "{\"weight\":" + String(weight) + ",\"temp1\":" + String(temp1) +
                     ",\"temp2\":" + String(temp2) + ",\"humidity1\":" + String(humidity1) +
                     ",\"humidity2\":" + String(humidity2) + "}";

    sim800.print("AT+MQTTPUB=0,\"beehive/data\",\"" + payload + "\",0,0");
    delay(100);
}

void CELLcheckForOTAUpdate() {
    sim800.println("AT+HTTPINIT");
    delay(100);
    sim800.println("AT+HTTPPARA=\"URL\",\"http://your-server.com/firmware.bin\"");
    delay(100);
    sim800.println("AT+HTTPACTION=0"); // Start GET request
    delay(5000);
    sim800.println("AT+HTTPREAD"); // Read the response
}

