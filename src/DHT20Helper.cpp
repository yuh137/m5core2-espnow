#include "DHT20Helper.h"
DHT20 DHT;

void setupDHT()
{
    DHT.begin();
}

float getTemperature()
{
    return DHT.getTemperature();
}

float getHumidity()
{
    return DHT.getHumidity();
}

void getSensorData(float& temperature, float& humidity)
{
    int status = DHT.read();
    if (status == DHT20_OK)
    {
        temperature = getTemperature();
        humidity = getHumidity();
    }
}