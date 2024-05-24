#pragma once
#include "global.h"

class MyMQTT
{
  private:
    String mqtt_server;
    String user;
    String password;
    WiFiClient espClient;
    char* payload;

  public:
    PubSubClient client;
    MyMQTT(String server, String user, String password)
        : mqtt_server(server),
          user(user),
          password(password),
          client(espClient),
          payload(nullptr)
    {
        client.setBufferSize(2048);
    }
    void connectToMQTT();    // Connect tới server bằng username đã tạo
    void subscribe(String);  // Subscribe đến feed
    void unsubscribe(String);
    bool publish(String, String);  // Publish đến feed
    void setPayload(char* newPayload);
    const char* getPayload()
    {
        if (this->payload == nullptr)
        {
            return nullptr;
        }
        size_t payloadLength = strlen(this->payload);
        char* res = new char[payloadLength + 1];
        memcpy(res, this->payload, payloadLength);
        res[payloadLength] = '\0';
        this->payload = nullptr;
        Serial.print("res in getPayload: ");
        Serial.println(res);

        return res;
    }
    void checkConnect();
    void nonBlockingConnect();

    void connectToServer();
    void testReconnectToMQTT();

  private:
    void callback(char*, byte*, unsigned int);
    void reConnect();
    bool reconnect();
};

extern char clientId[100];
extern long lastReconnectAttempt;