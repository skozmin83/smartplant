#ifndef ICONTROL_CENTER
#define ICONTROL_CENTER

#include "IDataPublisher.h"
#include "CentralData.h"

class IControlCenter {
public:
    virtual IDataPublisher &getPublisher() =0;
    virtual boolean publish(CenterData *data) = 0;
    virtual boolean loop() =0;
};

#endif
