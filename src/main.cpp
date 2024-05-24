#include <Arduino.h>
#include "logic.h"

// callback when data is sent

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    setupDHT();

    M5.begin(true, false, true, true, kMBusModeOutput, false);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_addPeer();

    esp_now_register_recv_cb(OnDataReceived);

    xTaskCreatePinnedToCore(mqttKeepAlive, "MQTT TASK", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(pubSubTask, "PUBSUB TASK", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(subTask, "SUB TASK", 4096, NULL, 2, NULL, 1);
}

void loop()
{
    // int status = DHT.read();
    // float temperature = DHT.getTemperature();
    // float humidity = DHT.getHumidity();

    // myData.temperature = temperature;
    // myData.humidity = humidity;

    // esp_err_t result = esp_now_send(peerAddress, (uint8_t*)&myData, sizeof(myData));

    // if (result == ESP_OK)
    // {
    //     Serial.println("Sent with success");
    // }
    // else
    // {
    //     Serial.println("Error sending the data");
    // }
    // delay(2000);
    sendData();
    delay(5000);
}
