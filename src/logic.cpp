#include "logic.h"
MyMQTT myMQTT(MQTT_SERVER, MQTT_USERNAME, MQTT_PASSWORD);

const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(2) + 200;
DynamicJsonDocument doc(capacity);
DynamicJsonDocument publishedDoc(capacity);

int cur_fan = 0;
int prv_fan = 0;
int cur_light = 0;
int prv_light = 0;

static const char* tmpPayload;

int32_t getWiFiChannel(const char* ssid)
{
    if (int32_t n = WiFi.scanNetworks())
    {
        for (uint8_t i = 0; i < n; i++)
        {
            if (!strcmp(ssid, WiFi.SSID(i).c_str()))
            {
                return WiFi.channel(i);
            }
        }
    }
    return 0;
}

void eventWiFi(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
        case ARDUINO_EVENT_WIFI_STA_START:
            log_i("M5 Supermarket station mode started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            log_i("M5 Supermarket station mode stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            log_i("M5 Supermarket Access Point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            log_i("M5 Supermarket Access Point stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            log_i("[WIFI] Connected");
            log_i("Channel: %d", info.wifi_sta_connected.channel);
            // WiFi.printDiag(Serial);  // Uncomment to verify channel number before
            // esp_wifi_set_promiscuous(true);
            // channel = WiFi.channel();
            // esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
            // esp_wifi_set_promiscuous(false);
            // WiFi.printDiag(Serial);
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            log_i("A client just connect to this AP %s", WiFi.softAPIP().toString().c_str());
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            log_i("A client just disconnect from the AP");
            break;
        default:
            break;
    }
}

void monitorWiFi()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.onEvent(eventWiFi);
}

void connectToWiFi()
{
    log_i("[WIFI] Establishing connection");
    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        log_i("[WIFI] Connecting to: %s", (const char*)WIFI_SSID);
        if (WiFi.status() == WL_CONNECTED)
        {
            log_i("Channel: %ld", WiFi.channel());
            break;
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

const char* extractWord(const char* input)
{
    const char* start = strchr(input, '\"');
    if (start == nullptr)
    {
        return nullptr;
    }

    const char* end = strchr(start + 1, '\"');
    if (end == nullptr)
    {
        return nullptr;
    }

    size_t length = end - start - 1;
    char* result = new char[length + 1];
    strncpy(result, start + 1, length);
    result[length] = '\0';
    return result;
}

char* createDeviceJson(const char* device_key, int current, int voltage, int power, bool status)
{
    publishedDoc["data_device"]["device_key"] = device_key;
    publishedDoc["data_device"]["current"] = current;
    publishedDoc["data_device"]["voltage"] = voltage;
    publishedDoc["data_device"]["power"] = power;
    publishedDoc["data_device"]["status"] = status;

    char* jsonString = new char[measureJson(publishedDoc) + 1];  // +1 for the null terminator
    serializeJson(publishedDoc, jsonString, measureJson(publishedDoc) + 1);

    return jsonString;
}

void pubLogic()
{
    char temp_char[10];
    char humid_char[10];

    sprintf(temp_char, "%.2f", outgoingSetpoints.temperature);
    sprintf(humid_char, "%.2f", outgoingSetpoints.humidity);

    StaticJsonDocument<500> doc;
    JsonArray data_sensor = doc.createNestedArray("data_sensor");

    JsonObject temperature = data_sensor.createNestedObject();
    temperature["sensor_key"] = "temperature";
    temperature["sensor_value"] = atof(temp_char);

    JsonObject humidity = data_sensor.createNestedObject();
    humidity["sensor_key"] = "humidity";
    humidity["sensor_value"] = atof(humid_char);

    String output;
    serializeJson(doc, output);

    myMQTT.publish(FEED_SENSOR, output);
}

void subLogic()
{
    tmpPayload = myMQTT.getPayload();
    if (tmpPayload != NULL)
    {
        log_i("Payload received in logic: %s", tmpPayload);
        const char* type = extractWord(tmpPayload);
        log_i("type: %s", type);

        DeserializationError error = deserializeJson(doc, tmpPayload);
        if (error)
        {
            log_i("deserializeJson() failed: %s", error.c_str());
            return;
        }

        if (strcmp(type, "data_device") == 0)
        {
            JsonObject data_device = doc["data_device"];

            const char* device_key = data_device["device_key"];
            int current = data_device["current"];
            int voltage = data_device["voltage"];
            int power = data_device["power"];
            int status = data_device["status"];

            Serial.print(F("Devices Key: "));
            Serial.println(device_key);
            Serial.print(F("Current: "));
            Serial.println(current);
            Serial.print(F("Voltage: "));
            Serial.println(voltage);
            Serial.print(F("Power: "));
            Serial.println(power);
            Serial.print(F("Status: "));
            Serial.println(status);

            if (strcmp(device_key, "light") == 0)
            {
                relayAStatus = status;
                switchChanel_3();
                controlRelay1(relayAStatus);
            }
            else if (strcmp(device_key, "fan") == 0)
            {
                relayBStatus = status;
                switchChanel_3();
                controlRelay2(relayBStatus);
            }
        }
    }
}

void displayData()
{
    switchChanel_4();
    getSensorData(outgoingSetpoints.temperature, outgoingSetpoints.humidity);
    updateSensorField(outgoingSetpoints.temperature, outgoingSetpoints.humidity);
}
void relayControl()
{
    M5.update();
    if (M5.BtnA.wasPressed())
    {
        switchChanel_3();
        relayAStatus = 1 - relayAStatus;
        controlRelay1(relayAStatus);
        const char* deviceJson = createDeviceJson("light", -1, -1, -1, relayAStatus);
        myMQTT.publish(FEED_LIGHT, deviceJson);
        log_i("Relay 1: %d", relayAStatus);
    }
    if (M5.BtnB.wasPressed())
    {
        switchChanel_3();
        relayBStatus = 1 - relayBStatus;
        controlRelay2(relayBStatus);
        const char* deviceJson = createDeviceJson("fan", -1, -1, -1, relayBStatus);
        myMQTT.publish(FEED_FAN, deviceJson);
        log_i("Relay 2: %d", relayBStatus);
    }
    updateDevicesField(relayBStatus, relayAStatus);
}

void mqttKeepAlive(void* pvParameters)
{
    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            myMQTT.client.loop();
        }
        else
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                connectToWiFi();
            }
            myMQTT.testReconnectToMQTT();
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void subTask(void* pvParameters)
{
    for (;;)
    {
        subLogic();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pubTask(void* pvParameters)
{
    for (;;)
    {
        pubLogic();
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void screenTask(void* pvParameters)
{
    for (;;)
    {
        displayData();
        pubLogic();
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void relayTask(void* pvParameters)
{
    for (;;)
    {
        relayControl();
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
