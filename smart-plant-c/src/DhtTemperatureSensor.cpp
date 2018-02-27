#include "ITemperatureSensor.h"
#include <DHT.h>

#ifndef PLANT_TEMPERATURE_SENSOR_DHT
#define PLANT_TEMPERATURE_SENSOR_DHT

class DhtTemperatureSensor : public ITemperatureSensor {
private:
    char *id;
    DHT dht;
    TemperatureResult result = TemperatureResult();
private:
    float readTemperature() {
        float curTemp = -274.0f;
        for (int i = 0; i < 10; i++) {
            curTemp = dht.readTemperature();
            if (!isnan(curTemp)) {
                break;
            } else {
                Serial.println("curTemp is nan, re-measure #" + String(i));
                delay(2000);
            }
        }
        return curTemp;
    }

    float readHumidity() {
        float curHumidity = -1.0f;
        for (int i = 0; i < 10; i++) {
            curHumidity = dht.readHumidity();
            if (!isnan(curHumidity)) {
                break;
            } else {
                Serial.println("curHumidity is nan, re-measure #" + String(i));
                delay(2000);
            }
        }
        return curHumidity;
    }

public:
    DhtTemperatureSensor(uint8_t pin, uint8_t type, char *id) : dht(pin, type), id(id) {}

    ITemperatureResult &read() override {
        result.setHumidity(readHumidity());
        result.setTemperature(readTemperature());
        return result;
    }

    void init() override {
        dht.begin();
    }

    char *getId() override {
        return id;
    }
};

#endif
