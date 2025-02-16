#ifndef CELL_H
#define CELL_H

void setupcell();
void checkSIM();
void sendSMS();
void setupGPRS();
void CELLconnectToMQTT();
void publishData(float weight, float temp1, float temp2, float humidity1, float humidity2);
void CELLcheckForOTAUpdate();
#endif
