
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "IConnector.h"

#ifndef WIFI_OTA_CONNECTOR
#define WIFI_OTA_CONNECTOR

class ConnectorWifiOTA : public IConnector {
private:
    const char *ssid;
    const char *wifiPassword;
    const char *host = "OTA-LEDS";
    const uint8_t dimmer_pin[3] = {14, 5, 15};
public:
    ConnectorWifiOTA(const char *ssid,
                     const char *wifiPassword) {
        ConnectorWifiOTA::ssid = ssid;
        ConnectorWifiOTA::wifiPassword = wifiPassword;
    }

    void *operator new(size_t size) { return malloc(size); }
    void operator delete(void *ptr) { free(ptr); }

    void init() override {
        connect();
        Serial.println("Configuring OTA. ");
        /* configure dimmers, and OTA server events */
        analogWriteRange(1000);
        analogWrite(LED_BUILTIN, 990);
        for (uint8_t pin : dimmer_pin) {
            pinMode(pin, OUTPUT);
            analogWrite(pin, 50);
        }

        ArduinoOTA.setHostname(host);
        ArduinoOTA.onStart([this]() { // switch off all the PWMs during upgrade
            for (uint8_t pin : this->dimmer_pin) {
                analogWrite(this->dimmer_pin[pin], 0);
                analogWrite(LED_BUILTIN, 0);
            }
        });

        ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
            for (int i = 0; i < 30; i++) {
                analogWrite(LED_BUILTIN, (i * 100) % 1001);
                delay(50);
            }
        });

        ArduinoOTA.onError([](ota_error_t error) {
            (void) error;
            ESP.restart();
        });

        /* setup the OTA server */
        ArduinoOTA.begin();
        Serial.println("OTA configured. ");
    }

    void loop() override {
        ArduinoOTA.handle();
    }

    void connect() override {
        while (WiFi.status() != WL_CONNECTED) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
            digitalWrite(LED_BUILTIN, HIGH);
            setup_wifi();
        }
    }

    bool isConnected() override {
        return WiFi.status() == WL_CONNECTED;
    }

    void disconnect() override {
        Serial.println("Closing WiFi connection...");
        WiFi.disconnect();
    }

private:
    void setup_wifi() {
        delay(10);
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

//        WiFi.mode(WIFI_STA);
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