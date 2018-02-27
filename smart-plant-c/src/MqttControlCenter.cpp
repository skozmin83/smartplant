#ifndef MQTT_CONTROL_CENTER
#define MQTT_CONTROL_CENTER

#define MQTT_MAX_PACKET_SIZE 256
//MQTT_KEEPALIVE
//Sets the keepalive interval, in seconds, the client will use. This is used to maintain the connection when no other packets are being sent or received.
//Default: 15 seconds
//
//MQTT_VERSION
//Sets the version of the MQTT protocol to use.
//Default: MQTT 3.1.1
//
//MQTT_MAX_TRANSFER_SIZE
//        Sets the maximum number of bytes passed to the network client in each write call. Some hardware has a limit to how much data can be passed to them in one go, such as the Arduino Wifi Shield.
//Default: undefined (complete packet passed in each write call)
//
//MQTT_SOCKET_TIMEOUT
//        Sets the timeout when reading from the network. This also applies as the timeout for calls to connect.
//Default: 15 seconds

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include "IControlCenter.h"
#include "SerialPublisher.cpp"
#include "MqttDataJsonPublisher.cpp"
#include "IConnector.h"
#include <cstdlib> // for malloc and free

class MqttControlCenter : public IControlCenter {
private:
    const char *connectionId;
    const char *mqttUsername;
    const char *mqttPassword;

    WiFiClient wifiClient;
    PubSubClient mqttClient;
    MqttDataJsonPublisher publisher;
    IConnector *connector;

    char *topic;
    char *controllerId;
    char *sensorId;
    float temperature;
    float humidity;
    float mass;
    float voltage;
public:
    MqttControlCenter(const char *connectionId,
                      const char *mqttServer,
                      uint16_t mqttPort,
                      const char *mqttUsername,
                      const char *mqttPassword,
                      IConnector *connectorIn
    ) :
            connectionId(connectionId),
            mqttUsername(mqttUsername),
            mqttPassword(mqttPassword),
            wifiClient(WiFiClient()),
            mqttClient(PubSubClient(wifiClient)),
            publisher(MqttDataJsonPublisher(mqttClient)) {
        mqttClient.setServer(mqttServer, mqttPort);
        connector = connectorIn;
    }

    void *operator new(size_t size) { return malloc(size); }
    void operator delete(void *ptr) { free(ptr); }

    IDataPublisher &getPublisher() override {
        if (!mqttClient.connected()) {
            reconnect();
        }
//        publishTemp();
//        disconnect();
//        Serial.println( "Sleeping for a minute");
//        delay(SLEEP_DELAY_IN_SECONDS * 1000);
        return (IDataPublisher &) publisher;
    };

    boolean publish(char *baseTopic,
                    char *controllerId,
                    char *sensorId,
                    float temperature,
                    float humidity,
                    float mass,
                    float voltage) override {
        // todo use command pattern here
        this->topic = baseTopic;
        this->controllerId = controllerId;
        this->sensorId = sensorId;
        this->temperature = temperature;
        this->humidity = humidity;
        this->mass = mass;
        this->voltage = voltage;
        return retry(&MqttControlCenter::publishInternal, 10, "mqtt client publish call");
    }

    boolean loop() override {
        return retry(&MqttControlCenter::loopInternal, 10, "mqtt client loop call");
    }

    boolean loopInternal() {
        return mqttClient.loop();
    }

    boolean publishInternal() {
        return publisher.publish(topic, controllerId, sensorId, temperature, humidity, mass, voltage);
    }

    boolean retry(boolean(MqttControlCenter::*callback)(), int retryCount, const char *description) {
        auto status = static_cast<boolean>(false);
        for (int i = 0; i < retryCount; i++) {
            status = (this->*callback)();
            if (status) {
                break;
            } else {
                Serial.print("Issues with [");
                Serial.print(description);
                Serial.print("], ret code [");
                Serial.print(status);
                Serial.print("], state [");
                Serial.print(mqttClient.state());
                Serial.print("], retry: ");
                Serial.println(i);
                if (!mqttClient.connected()) {
                    reconnect();
                }
            }
        }
        return status;
    }

    void reconnect() {
        connector->connect();
        while (!mqttClient.connected()) {
            Serial.print("Attempting MQTT connection...");
            //if (client.connect("ESP8266_Client", mqtt_username, mqtt_password)) {
            //if (client.connect("ESP8266_Client")) {
            //mqttClient.connect("myClientID", willTopic, willQoS, willRetain, willMessage);
            //if (mqttClient.connect("client")) {
            if (mqttClient.connect(connectionId, mqttUsername, mqttPassword)) {
                Serial.println("connected");
//                mqttClient.subscribe(mqttSubTopic);
            } else {
                Serial.print("MQTT connection failed, mqtt client state: ");
                Serial.print(mqttClient.state());
                Serial.println(", try again in 5 seconds");
                delay(5000);
            }
        }
    }

    void disconnect() {
        Serial.println("Closing MQTT connection...");
        mqttClient.disconnect();
        mqttClient.loop();
        Serial.println("Closing WiFi connection...");
        connector->disconnect();
    }
};

#endif
