extern "C" {
#include "user_interface.h"
}

#include "MqttControlCenter.cpp"
#include "DhtTemperatureSensor.cpp"
//#include "ConnectorWifiOTA.cpp"
#include "ConnectorWifi.cpp"
#include "HX711.h"
#include "Wire.h"
#include "FDC1004.h"

#define PIN_DOUT            D7
#define PIN_CLK             D8

#define PIN_BUTTON          D7
#define PIN_TEMPERATURE     D3

ADC_MODE(ADC_VCC);

static const int MEASURE_INTERVAL_MILLIS = 60000;
char macAddress[18] = {0};

const char *ssid = "E7EA3E";
const char *password = "79296267";

const char *mqttServer = "raspberrypi";
const char *mqttUsername = "user";
const char *mqttPassword = "yourpassword";
//const char *mqttServer = "sergeypc";
//const char *mqttUsername = "user";
//const char *mqttPassword = "yourpassword";

char *mqttPubBase = (char *) "/devices";
char *mqttSubBase = (char *) "/control";

float calibration_factor = 1;
IConnector *connector;
IControlCenter *center;
DhtTemperatureSensor *temperatureSensor;
HX711 *massSensor;
uint32_t lastPublishTime = 0;
CenterData data;
double caps[] = {0, 0, 0, 0};
uint8_t capdacs[] = {0, 0, 0, 0};

char *readMac(char *toRead) {
    uint8_t mac[6];
    wifi_get_macaddr(STATION_IF, mac);
    sprintf(toRead, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return toRead;
}

void setup() {
    Wire.begin();
//    pinMode(PIN_BUTTON, INPUT);
//    pinMode(BUILTIN_LED, OUTPUT);
//    digitalWrite(BUILTIN_LED, HIGH);
    Serial.begin(115200);
    Serial.println("Start: ");
    uint8_t initRet = FDC1004_init();
    if (initRet) {
        Serial.printf("FDC1004 init failed: %d\n", initRet);
    }

    data.caps = caps;
    data.capdacs = capdacs;
    data.baseTopic = mqttPubBase;
    data.controllerId = macAddress;
    data.sensorId = const_cast<char *>("m1");
//    try {
        connector = new ConnectorWifi(ssid, password);
        center = new MqttControlCenter(readMac(macAddress), mqttServer, 1883, mqttUsername, mqttPassword, connector);
        temperatureSensor = new DhtTemperatureSensor(PIN_TEMPERATURE, DHT22, (char *) "dht22");
        massSensor = new HX711(PIN_DOUT, PIN_CLK);

        temperatureSensor->init();
        connector->init();
        massSensor->set_scale(calibration_factor); //Adjust to this calibration factor
//    } catch (...) {
//         todo add catch and reload
//        Serial.println("Error during init. ");
//        delay(500);
//        ESP.restart();
//    }
}

void loop() {
    uint32_t currenttime = millis();
    if ((currenttime - lastPublishTime) > MEASURE_INTERVAL_MILLIS) {
//        uint32_t chipId = ESP.getChipId();
//        uint32_t flashChipId = ESP.getFlashChipId();
        float voltage = ESP.getVcc() / 1023.0f;
        float mass = massSensor->get_units(10);
        ITemperatureResult &temperatureResult = temperatureSensor->read();

//        memset(caps, 0, sizeof(caps));
        for (uint8_t i = 0; i < 4; ++i) {
            FDC1004_measure_channel(i, &(caps[i]), &(capdacs[i]));
            Serial.printf(", channel %d|%f|%d\t", i, caps[i], capdacs[i]);
        }

        data.temperature = temperatureResult.getTemperature();
        data.humidity = temperatureResult.getHumidity();
        data.mass = mass;
        data.voltage = voltage;
//        String pubStatus = String("Publish status: ");
        boolean status = center->publish(&data);
//        pubStatus += String(status);
        boolean loopStatus = center->loop();
//        Serial.println(pubStatus + String(", overall: ") + String(loopStatus));
        lastPublishTime = millis();
    } else {
        center->loop();
//        int buttonStatus = digitalRead(D5);
//        if (HIGH == buttonStatus) {
//            Serial.print("Reset tare. ");
//            Serial.println(buttonStatus);
//            lastPublishTime = 0;
//            massSensor->tare(10);
//        }
    }
    connector->loop();
}
