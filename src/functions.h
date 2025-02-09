#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void initSerial();
void initPreferences();
void loadPreferences();
void initDHTSensors();
void initScale();
void connectToWiFi();
void createAccessPointIfNeeded();
void readDHTSensors();
void measureBattery();
void updateScale();
float movingAverage();
void tareScale();
void recalibrateScale();
void handleSerialCommands();
void WifiManagerinit();
void checkforWifi();
void enterDeepSleep();
void turnOffWiFi();
void messageHandler(char* topic, byte* payload, unsigned int length);
void enterNap();
void clearPreferences();
void setupI2S();
float detectBeeBuzzFrequency();
#endif