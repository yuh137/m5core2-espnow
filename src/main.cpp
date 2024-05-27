#include <Arduino.h>
#include "logic.h"

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    M5.begin(true, false, true, true, kMBusModeOutput, false);
    setupComponents();
    setupScreen();
    monitorWiFi();
    setup_espnow();

    xTaskCreatePinnedToCore(mqttKeepAlive, "MQTT TASK", 4096, NULL, 3, NULL, 0);
    // xTaskCreatePinnedToCore(pubTask, "PUBSUB TASK", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(subTask, "SUB TASK", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(screenTask, "SCREEN TASK", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(relayTask, "RELAY TASK", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(pairTask, "ESPNOW PAIRING TASK", 4096, NULL, 2, NULL, 1);
}

void loop()
{
    // sendData();
    // delay(5000);
}
