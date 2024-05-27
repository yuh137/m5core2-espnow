#pragma once

#define MQTT_SERVER   "mqttserver.tk"
#define MQTT_PORT     1883
#define MQTT_USERNAME "innovation"
#define MQTT_PASSWORD "Innovation_RgPQAZoA5N"
// #define FEED_TEMP "trungdai/feeds/temp"
// #define FEED_HUMID "trungdai/feeds/humid"
// #define FEED_FAN "trungdai/feeds/fan"
// #define FEED_LIGHT "trungdai/feeds/led"
#define FEED_SENSOR   "/innovation/airmonitoring/smarthome/sensors"
#define FEED_FAN      "/innovation/airmonitoring/smarthome/fan"
#define FEED_LIGHT    "/innovation/airmonitoring/smarthome/light"

#define WIFI_SSID     "ACLAB"
#define WIFI_PASS     "ACLAB2023"

#include <M5Core2.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <esp_now.h>
#include <PubSubClient.h>
#include "DHT20.h"
#include <Unit_4RELAY.h>
#include <ClosedCube_TCA9548A.h>
#include <esp_wifi.h>
