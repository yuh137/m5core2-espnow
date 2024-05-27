#include "../include/setup_espnow.h"
// uint8_t peerAddress[] = {0x34, 0x98, 0x7A, 0x6C, 0x60, 0xB0}; // LilyGo-HUY
uint8_t peerAddress[] = {0x34, 0x98, 0x7A, 0x6C, 0x60, 0xCC};  // LilyGo-Trung

MessageType messageType;
data_packet incomingReadings;
data_packet outgoingSetpoints;
pairing_packet pairingData;
PairingStatus pairingStatus = NOT_PAIRED;
esp_now_peer_info_t peerInfo;
int wifiChannel = 0;

void setup_espnow()
{
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}

void setup_add_peer(const uint8_t* mac)
{
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = WiFi.channel();
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        log_i("[ESPNOW] Failed to add peer");
        return;
    }
}

const char* getPeerMac(const uint8_t* mac_addr)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    return macStr;
}

void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status)
{
    log_i("[ESPNOW] Temperature: %0.2f, Humidity: %0.2f", outgoingSetpoints.temperature, outgoingSetpoints.humidity);
    log_i("[ESPNOW] Last Packet Send Status: %s", status == ESP_NOW_SEND_SUCCESS ? "Delivery Sucess" : "Delivery Fail");
    log_i("[ESPNOW] Last Packet Sent to: %s", getPeerMac(mac_addr));
}

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len)
{
    // log_i("%d bytes of data received from: %s", len, getPeerMac(mac));
    // StaticJsonDocument<1000> root;
    // String payload;
    // uint8_t type = incomingData[0];  // first message byte is the type of message
    // switch (type)
    // {
    //     case DATA:
    //         log_i("Bytes received: %d", len);
    //         break;
    //     case PAIRING:
    //         memcpy(&pairingData, incomingData, sizeof(pairingData));
    //         log_i("MsgType: %s, ID: %d", pairingData.msgType == PAIRING ? "PAIRING" : "DATA", pairingData.id);
    //         log_i("Pairing request from: %s", getPeerMac(mac));
    //         log_i("Pairing channel: %d", pairingData.channel);
    //         if (pairingData.id > 0 && pairingData.msgType == PAIRING)
    //         {
    //             pairingData.id = 0;
    //             pairingData.channel = wifiChannel;
    //             esp_err_t result = esp_now_send(mac, (uint8_t*)&pairingData, sizeof(pairingData));
    //             log_i("[ESPNOW] Sent response to %s", mac);
    //             setup_add_peer(mac);
    //         }
    //         break;
    //     default:
    //         break;
    // }
}

void readDataToSend()
{
    outgoingSetpoints.msgType = DATA;
    outgoingSetpoints.id = 0;
    getSensorData(outgoingSetpoints.temperature, outgoingSetpoints.humidity);
}

void pairTask(void* pvParameters)
{
    for (;;)
    {
        initConnection();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void initConnection()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        pairingData.msgType = PAIRING;
        pairingData.id = 0;
        pairingData.channel = WiFi.channel();
        esp_err_t result = esp_now_send(peerAddress, (uint8_t*)&pairingData, sizeof(pairingData));
        if (result)
        {
            log_i("[ESPNOW] Sent channel to %d", peerAddress);
            // setup_add_peer(peerAddress);
        }
        else
            log_i("[ESPNOW] Failed to send");
    }
}
