#ifndef SERIAL_PUBLISHER
#define SERIAL_PUBLISHER

#include "IDataPublisher.h"
#include "IControlCenter.h"
#include <Arduino.h>

class SerialPublisher : public IDataPublisher {
public:
    SerialPublisher() {
        Serial.begin(115200);
    }
    boolean publish(CenterData *data) override {
        if (isnan(data->mass)) {
            Serial.print(": controller: ");
            Serial.print(data->controllerId);
            Serial.print(": sensor: ");
            Serial.print(data->sensorId);
            Serial.print(": mass: ");
            Serial.print(data->mass);
            Serial.print(": Voltage: ");
            Serial.print(data->voltage);
        } else {
            Serial.println("Failed to read from sensor.");
        }
        return (boolean) true;
    }

private:
};

#endif
