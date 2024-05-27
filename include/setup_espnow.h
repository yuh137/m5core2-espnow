#pragma once

#include "setup.h"

enum MessageType
{
    PAIRING,
    DATA,
};
extern MessageType messageType;

struct data_packet
{
    uint8_t msgType;
    uint8_t id;
    float temperature;
    float humidity;
};
extern data_packet incomingReadings;
extern data_packet outgoingSetpoints;

struct pairing_packet
{
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
};
extern pairing_packet pairingData;

enum PairingStatus
{
    NOT_PAIRED,
    PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};
extern PairingStatus pairingStatus;

extern int wifiChannel;

void setup_espnow();
void setup_add_peer(const uint8_t*);
void OnDataSent(const uint8_t*, esp_now_send_status_t);
void OnDataRecv(const uint8_t*, const uint8_t*, int);

void pairTask(void*);
void initConnection();