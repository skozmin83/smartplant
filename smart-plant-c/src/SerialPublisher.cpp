#ifndef SERIAL_PUBLISHER
#define SERIAL_PUBLISHER

#include "IDataPublisher.h"
#include <Arduino.h>

class SerialPublisher : public IDataPublisher {
public:
    SerialPublisher() {
        Serial.begin(115200);
    }
    boolean publish(char *baseTopic, char *controllerId, char *sensorId, float mass, float voltage, float d, float d1) {
        if (isnan(mass)) {
            Serial.print(": controller: ");
            Serial.print(controllerId);
            Serial.print(": sensor: ");
            Serial.print(sensorId);
            Serial.print(": mass: ");
            Serial.print(mass);
            Serial.print(": Voltage: ");
            Serial.print(voltage);
        } else {
            Serial.println("Failed to read from sensor.");
        }
        return (boolean) true;
    }

private:
};

#endif
