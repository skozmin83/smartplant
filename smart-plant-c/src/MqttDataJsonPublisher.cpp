#ifndef MQTT_PUBLISHER
#define MQTT_PUBLISHER

#include <Arduino.h>
#include <PubSubClient.h>
#include "IDataPublisher.h"

class MqttDataJsonPublisher : public IDataPublisher {
private:
    PubSubClient &mqttClient;
    char topicBuf[256];
    char payloadBuf[256];
    char float_temperature[8]; // consider - sign
    char float_humidity[8]; // consider - sign
    char float_mass[8];// consider - sign
    char float_voltage[8];
public:
    MqttDataJsonPublisher(PubSubClient &client) : mqttClient(client) {}

    boolean publish(char *baseTopic,
                    char *controllerId,
                            char *sensorId,
                            float temperature,
                            float humidity,
                            float mass,
                            float voltage) override {
        memset(payloadBuf, '\0', sizeof(payloadBuf));
        memset(topicBuf, '\0', sizeof(topicBuf));

        strcpy(topicBuf, baseTopic);
        strcat(topicBuf, "/");
        strcat(topicBuf, controllerId);
        strcat(topicBuf, "/");
        strcat(topicBuf, sensorId);

        sprintf(payloadBuf, R"({"t":%s,"h":%s,"m":%s,"v":%s,"sid":"%s"})",
                dtostrf((isnan(temperature) ? -1 : temperature), 4, 2, float_temperature),
                dtostrf((isnan(humidity) ? -1 : humidity), 4, 2, float_humidity),
                dtostrf((isnan(mass) ? -1 : mass), 4, 3, float_mass),
                dtostrf(voltage, 4, 2, float_voltage),
//                controllerId,
                sensorId
        );
        Serial.print(topicBuf);
        Serial.print(": ");
        Serial.println(payloadBuf);
        return mqttClient.publish(topicBuf, payloadBuf);
//        mqttClient.loop();
//        return (boolean)true;
    }
};

#endif
