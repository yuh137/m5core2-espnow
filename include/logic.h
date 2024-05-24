#pragma once
#include "global.h"
#include "MQTTHelper.h"

void initM5Display();
void publishSensorData(float, float);
void pubSubTask(void *);
void mqttKeepAlive(void *);
void connectToWiFi();
void pubSubLogic();
void subLogic();
void subTask(void *);
void setupDHT();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len);
void esp_now_addPeer();
void sendData();