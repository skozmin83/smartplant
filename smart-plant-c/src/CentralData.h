#ifndef CENTER_DATA
#define CENTER_DATA

struct CenterData {
    char *baseTopic = 0;
    char *controllerId = 0;
    char *sensorId = 0;
    float temperature = 0;
    float humidity = 0;
    float mass = 0;
    double *caps = 0;
    uint8_t *capdacs = 0;
    float voltage = 0;
};
typedef struct CenterData CenterData;

#endif
