#include "MQTTHelper.h"

char clientId[100];
void MyMQTT::connectToMQTT()
{
    client.setServer(mqtt_server.c_str(), MQTT_PORT);
    srand((unsigned)time(NULL));
    int random = rand();
    sprintf(clientId, "clientId_%d", random);
    this->reconnect();
    // reConnect();
    client.setCallback([this](char* topic, byte* payload, unsigned int length)
                       { this->callback(topic, payload, length); });
}

void MyMQTT::connectToServer()
{
    client.setServer(mqtt_server.c_str(), MQTT_PORT);
    srand((unsigned)time(NULL));
    int random = rand();
    sprintf(clientId, "clientId_%d", random);
}

void MyMQTT::testReconnectToMQTT()
{
    client.setServer(mqtt_server.c_str(), MQTT_PORT);
    srand((unsigned)time(NULL));
    int random = rand();
    sprintf(clientId, "clientId_%d", random);
    const char* str = "ESP32Client";
    while (!client.connected())
    {
        client.connect(clientId, user.c_str(), password.c_str());
        log_i("Connecting to MQTT server...");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    log_i("[MQTT] Connected");
    log_i("Connected with clientid %s", clientId);
    // client.subscribe(FEED_TEMP);
    // client.subscribe(FEED_HUMID);
    client.subscribe(FEED_FAN);
    client.subscribe(FEED_LIGHT);

    // this->reconnect();
    // reConnect();
    client.setCallback([this](char* topic, byte* payload, unsigned int length)
                       { this->callback(topic, payload, length); });
}

void MyMQTT::subscribe(String feedName)
{
#ifdef ADAFRUIT
    String topic = user + "/feeds/" + feedName;
#endif
    String topic = feedName;

    client.subscribe(topic.c_str(), 1);
}

void MyMQTT::unsubscribe(String feedName)
{
#ifdef ADAFRUIT
    String topic = user + "/feeds/" + feedName;
#endif
    String topic = feedName;

    client.unsubscribe(topic.c_str());
}

bool MyMQTT::publish(String feedName, String message)
{
#ifdef ADAFRUIT
    String topic = user + "/feeds/" + feedName;
#endif
    String topic = feedName;

    Serial.print("Publishing to topic: ");
    Serial.println(feedName);
    Serial.print("Status: ");
    if (client.publish(topic.c_str(), message.c_str(), 1))
    {
        Serial.println("Success!");
        return 1;
    }
    Serial.println("Failed!");
    return 0;
}
int pub = 1;
void MyMQTT::checkConnect()
{
    if (!client.connected())
    {
        Serial.println("MQTT Connection lost!");
        reConnect();
    }
    client.loop();
}

void MyMQTT::callback(char* topic, byte* payload, unsigned int length)
{
    unsigned int newLength = strlen(topic) + length + 1;
    char* result = new char[newLength];
    // Serial.printf("length: %d\n", strlen(topic));
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    memcpy(result, payload, length);
    result[length] = '\0';
    setPayload(result);
    Serial.println();
    Serial.print("payload in MyMQTT::callback ");
    Serial.println(this->payload);

    // memcpy(result, topic, strlen(topic));
    // strcat(result, (const char*)payload);
    // result[newLength] = '\0';
    // setPayload(result);
    // Serial.printf("\npayload in MyMQTT::callback ");
    // Serial.println(this->payload);
}

void MyMQTT::reConnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        Serial.print("Wifi status: ");
        Serial.println(WiFi.status());
        if (client.connect("ESP32Clie12321", user.c_str(), password.c_str()))
        {
            Serial.println("connected");
            // client.subscribe(FEED_TEMP);
            // client.subscribe(FEED_HUMID);
            client.subscribe(FEED_SENSOR);
            client.subscribe(FEED_FAN);
            client.subscribe(FEED_LIGHT);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

long lastReconnectAttempt = 0;
void MyMQTT::nonBlockingConnect()
{
    if (!client.connected())
    {
        Serial.print("MQTT state: ");
        Serial.println(client.state());
        Serial.println("Attempting MQTT connection...");
        long now = millis();
        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            if (reconnect())
            {
                lastReconnectAttempt = 0;
            }
        }
    }
    else
    {
        client.loop();
    }
}

bool MyMQTT::reconnect()
{
    if (client.connect(clientId, user.c_str(), password.c_str()))
    {
        Serial.print("Connected with clientd: ");
        Serial.println(clientId);
        client.setKeepAlive(30);
        client.subscribe(FEED_SENSOR);
        client.subscribe(FEED_FAN);
        client.subscribe(FEED_LIGHT);
    }
    else
    {
        Serial.print("Attempted connect with clientId: ");
        Serial.print(clientId);
        Serial.println(" failed");
    }
    return client.connected();
}

void MyMQTT::setPayload(char* newPayload)
{
    if (this->payload != nullptr)
    {
        delete[] this->payload;
    }
    size_t lengthPayload = strlen(newPayload);
    this->payload = new char[lengthPayload + 1];
    memcpy(this->payload, newPayload, lengthPayload);
    this->payload[lengthPayload] = '\0';
}