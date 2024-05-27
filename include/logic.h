#pragma once
#include "global.h"
#include "MQTTHelper.h"
#include "setup.h"
#include "setup_espnow.h"

int32_t getWiFiChannel(const char* ssid);
void monitorWiFi();
void connectToWiFi();
void displayData();
void subLogic();
void pubLogic();
void relayControl();

void screenTask(void*);
void subTask(void*);
void pubTask(void*);
void mqttKeepAlive(void*);
void relayTask(void*);