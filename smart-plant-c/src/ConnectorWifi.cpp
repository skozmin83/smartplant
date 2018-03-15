
#include "IConnector.h"

#ifndef WIFI_CONNECTOR
#define WIFI_CONNECTOR

#include <WiFiClient.h>
#include <ESP8266WiFi.h>

class ConnectorWifi : public IConnector {
private:
    const char *ssid;
    const char *wifiPassword;
public:
    ConnectorWifi(const char *ssid,
                  const char *wifiPassword) {
        ConnectorWifi::ssid = ssid;
        ConnectorWifi::wifiPassword = wifiPassword;
    }
    void *operator new(size_t size) { return malloc(size); }
    void operator delete(void *ptr) { free(ptr); }
    void connect() override {
        while (WiFi.status() != WL_CONNECTED) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
            digitalWrite(LED_BUILTIN, HIGH);
            setup_wifi();
        }
    }

    bool isConnected() override {
        return false;
    }

    void disconnect() override {
        Serial.println("Closing WiFi connection...");
        WiFi.disconnect();
    }
    void loop() override {}
    void init() override {}
private:
    void setup_wifi() {
        delay(10);
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, wifiPassword);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        Serial.print("WiFi connected, IP address: ");
        Serial.println(WiFi.localIP());
    }
};

#endif