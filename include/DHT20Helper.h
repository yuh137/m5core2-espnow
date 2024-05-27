#pragma once

#include "global.h"

void setupDHT();
float getTemperature();
float getHumidity();
void getSensorData(float&, float&);