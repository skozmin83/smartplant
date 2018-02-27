#ifndef SMART_PLANT_C_ICONNECTOR_H
#define SMART_PLANT_C_ICONNECTOR_H

class IConnector {
public:
    virtual void init() =0;
    virtual void loop() =0;
    virtual void connect() =0;
    virtual bool isConnected() =0;
    virtual void disconnect() =0;
};
#endif //SMART_PLANT_C_ICONNECTOR_H
