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




#endif