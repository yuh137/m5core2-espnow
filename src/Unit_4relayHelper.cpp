#include "../include/Unit_4RelayHelper.h"

UNIT_4RELAY relay;

bool syn_asyn_flag = SYNCHRONOUS;
bool all_flag = false;
uint8_t relayState = OFF_RELAY_1;
uint8_t ALLrelayState = IDLE;
uint8_t relayAStatus = OFF_SIGNAL;
uint8_t relayBStatus = OFF_SIGNAL;
bool relayStatusA = false;
bool relayStatusB = false;

void setupRelay()
{
    relay.begin();
    relay.Init(0);
}

void controlRelay()
{
    M5.update();
    switch (relayState)
    {
        case SWITCH_FLAG:
            syn_asyn_flag = !syn_asyn_flag;
            relay.switchMode(syn_asyn_flag);
            break;
        case ON_RELAY_1:
            relay.relayWrite(RELAY_1, ON_SIGNAL);
            relay.ledWrite(RELAY_1, ON_SIGNAL);
            break;
        case OFF_RELAY_1:
            relay.relayWrite(RELAY_1, OFF_SIGNAL);
            relay.ledWrite(RELAY_1, OFF_SIGNAL);
            break;
        case OFF_RELAY_2:
            relay.relayWrite(RELAY_2, OFF_SIGNAL);
            relay.ledWrite(RELAY_2, OFF_SIGNAL);
            break;
        case ON_RELAY_2:
            relay.relayWrite(RELAY_2, ON_SIGNAL);
            relay.ledWrite(RELAY_2, ON_SIGNAL);
            break;
        case OFF_RELAY_3:
            relay.relayWrite(RELAY_3, OFF_SIGNAL);
            relay.ledWrite(RELAY_3, OFF_SIGNAL);
            break;
        case ON_RELAY_3:
            relay.relayWrite(RELAY_3, ON_SIGNAL);
            relay.ledWrite(RELAY_3, ON_SIGNAL);
            break;
        case OFF_RELAY_4:
            relay.relayWrite(RELAY_4, OFF_SIGNAL);
            relay.ledWrite(RELAY_4, OFF_SIGNAL);
            break;
        case ON_RELAY_4:
            relay.relayWrite(RELAY_4, ON_SIGNAL);
            relay.ledWrite(RELAY_4, ON_SIGNAL);
            break;
        default:
            printf("Invalid state!!");
            break;
    }
}

void controlRelay1(uint8_t signal)
{
    relay.relayWrite(RELAY_1, signal);
    relay.ledWrite(RELAY_1, signal);
}

void controlRelay2(uint8_t signal)
{
    relay.relayWrite(RELAY_2, signal);
    relay.ledWrite(RELAY_2, signal);
}

void controlALL()
{
    switch (ALLrelayState)
    {
        case IDLE:
            break;
        case OFF_ALL:
            relay.relayAll(OFF_SIGNAL);
            // relay.ledAll(OFF_SIGNAL);
            break;
        case ON_ALL:
            relay.relayAll(ON_SIGNAL);
            // relay.ledAll(ON_SIGNAL);
            break;
        default:
            printf("Invalid state!!");
            break;
    }
}