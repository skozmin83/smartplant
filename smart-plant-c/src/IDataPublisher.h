//
// Created by sergey on 3/4/2017.
//

#ifndef DATA_PUBLISHER_H
#define DATA_PUBLISHER_H

#include <Arduino.h>

class IDataPublisher {
public:
    virtual boolean
    publish(char *baseTopic, char *controllerId, char *sensorId, float mass, float voltage, float d, float d1) = 0;
private:
};

#endif //DATA_PUBLISHER_H
