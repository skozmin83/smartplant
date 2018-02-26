#ifndef ICONTROL_CENTER
#define ICONTROL_CENTER

#include "IDataPublisher.h"

class IControlCenter {
public:
    virtual IDataPublisher &getPublisher() =0;
    virtual boolean publish(char *baseTopic,
                            char *controllerId,
                            char *sensorId,
                            float temperature,
                            float humidity,
                            float mass,
                            float voltage) = 0;
    virtual boolean loop() =0;
};

#endif
