#ifndef MQTT_PUBLISHER
#define MQTT_PUBLISHER

#include <Arduino.h>
#include <PubSubClient.h>
#include "IDataPublisher.h"
#include "IControlCenter.h"

class MqttDataJsonPublisher : public IDataPublisher {
private:
    const char *pattern = R"({"t":%s,"h":%s,"m":%s,"v":%s,"c0":%s,"c1":%s,"c2":%s,"c3":%s,"cd0":%s,"cd1":%s,"cd2":%s,"cd3":%s,"sid":"%s"})";
    PubSubClient &mqttClient;
    char topicBuf[256];
    char payloadBuf[256];
    char float_temperature[8]; // consider - sign
    char float_humidity[8]; // consider - sign
    char float_mass[16];// consider - sign
    char float_voltage[8];
    char cap0[16];
    char cap1[16];
    char cap2[16];
    char cap3[16];
    char capdac0[2];
    char capdac1[2];
    char capdac2[2];
    char capdac3[2];
public:
    MqttDataJsonPublisher(PubSubClient &client) : mqttClient(client) {}

    boolean publish(CenterData *data) override {
        memset(payloadBuf, '\0', sizeof(payloadBuf));
        memset(topicBuf, '\0', sizeof(topicBuf));

        strcpy(topicBuf, data->baseTopic);
        strcat(topicBuf, "/");
        strcat(topicBuf, data->controllerId);
        strcat(topicBuf, "/");
        strcat(topicBuf, data->sensorId);

        
//        sizeof(myInts)/sizeof(int)
        sprintf(payloadBuf, pattern,
                dtostrf((isnan(data->temperature) ? -1 : data->temperature), 4, 2, float_temperature),
                dtostrf((isnan(data->humidity) ? -1 : data->humidity), 4, 2, float_humidity),
                dtostrf((isnan(data->mass) ? -1 : data->mass), 4, 3, float_mass),
                dtostrf(data->voltage, 4, 2, float_voltage),
                dtostrf(data->caps[0], 4, 4, cap0),
                dtostrf(data->caps[1], 4, 4, cap1),
                dtostrf(data->caps[2], 4, 4, cap2),
                dtostrf(data->caps[3], 4, 4, cap3),
                itoa(data->capdacs[0], capdac0, 10),
                itoa(data->capdacs[1], capdac1, 10),
                itoa(data->capdacs[2], capdac2, 10),
                itoa(data->capdacs[3], capdac3, 10),
//                controllerId,
                data->sensorId
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
