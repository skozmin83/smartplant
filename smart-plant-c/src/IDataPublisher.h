//
// Created by sergey on 3/4/2017.
//

#ifndef DATA_PUBLISHER_H
#define DATA_PUBLISHER_H

#include <Arduino.h>
#include "CentralData.h"

class IDataPublisher {
public:
    virtual boolean publish(CenterData *data) = 0;
private:
};

#endif //DATA_PUBLISHER_H
