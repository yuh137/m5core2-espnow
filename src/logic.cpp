#include "logic.h"
MyMQTT myMQTT(MQTT_SERVER, MQTT_USERNAME, MQTT_PASSWORD);
DHT20 DHT;

uint8_t peerAddress[] = {0x34, 0x98, 0x7A, 0x6C, 0x60, 0xB0};
// DHT20 DHT;

struct struct_message
{
    float temperature;
    float humidity;
};

struct_message myData;
struct_message recvData;

esp_now_peer_info_t peerInfo;

int cur_fan = 0;
int prv_fan = 0;
int cur_light = 0;
int prv_light = 0;

static const char *tmpPayload;

void setupDHT()
{
    DHT.begin();
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.printf("Temperature: %0.2f\n", myData.temperature);
    Serial.printf("Humidity: %0.2f", myData.humidity);
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print("Last Packet Sent to: ");
    Serial.println(macStr);
}

void OnDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&recvData, incomingData, sizeof(recvData));
    log_i("Bytes received: %d", len);
    log_i("Data received: Temperature: %0.2f, Humidity: %0.2f", recvData.temperature, recvData.humidity);
}

void esp_now_addPeer()
{
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }
}

void initM5Display()
{
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(5);

    M5.Lcd.setCursor(20, 20);
    M5.Lcd.printf("Temperature: ");
    M5.Lcd.setCursor(180, 20);
    M5.Lcd.printf("%.2f\n", 0.00);

    M5.Lcd.setCursor(20, 40);
    M5.Lcd.printf("Humidity: ");
    M5.Lcd.setCursor(180, 40);
    M5.Lcd.printf("%.2f\n", 0.00);

    M5.Lcd.setCursor(20, 60);
    M5.Lcd.printf("Fan");
    M5.Lcd.setCursor(180, 60);
    M5.Lcd.printf("%d", cur_fan); // Fan status

    M5.Lcd.setCursor(20, 80);
    M5.Lcd.printf("Light");
    M5.Lcd.setCursor(180, 80);
    M5.Lcd.printf("%d", cur_light); // Light status
}

void publishSensorData(float temp, float humid)
{
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setCursor(20, 20);
    M5.Lcd.printf("Temperature: ");
    M5.Lcd.setCursor(20, 40);
    M5.Lcd.printf("Humidity: ");
    M5.Lcd.setCursor(180, 20);
    M5.Lcd.printf("%.2f\n", temp);
    M5.Lcd.setCursor(180, 40);
    M5.Lcd.printf("%.2f\n", humid);
}

void connectToWiFi()
{
    log_i("[WIFI] Establishing connection");
    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        log_i("[WIFI] Connecting to: %s", (const char *)WIFI_SSID);
        if (WiFi.status() == WL_CONNECTED)
            break;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    log_i("[WIFI] Connected");
}

void mqttKeepAlive(void *pvParameters)
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

void pubSubLogic()
{
    int status = DHT.read();
    float temp = DHT.getTemperature();
    float humid = DHT.getHumidity();

    char humid_char[10];
    char temp_char[10];

    sprintf(humid_char, "%.2f", humid);
    sprintf(temp_char, "%.2f", temp);

    publishSensorData(temp, humid);
    // myMQTT.publish(FEED_HUMID, humid_char);
    // myMQTT.publish(FEED_TEMP, temp_char);

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
        log_i("Payload received: %s", tmpPayload);
    }
}

void subTask(void *pvParameters)
{
    for (;;)
    {
        subLogic();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void pubSubTask(void *pvParameters)
{
    for (;;)
    {
        pubSubLogic();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void sendData()
{
    int status = DHT.read();
    float temperature = DHT.getTemperature();
    float humidity = DHT.getHumidity();

    myData.temperature = temperature;
    myData.humidity = humidity;

    esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));

    if (result == ESP_OK)
    {
        Serial.println("Sent with success");
    }
    else
    {
        Serial.println("Error sending the data");
    }
    delay(2000);
}