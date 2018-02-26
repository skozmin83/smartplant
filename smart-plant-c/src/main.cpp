extern "C" {
#include "user_interface.h"
}

#include "MqttControlCenter.cpp"
#include "DhtTemperatureSensor.cpp"
#include "HX711.h"

#define PIN_DOUT            D2
#define PIN_CLK             D3
#define PIN_BUTTON          D5
#define PIN_TEMPERATURE     D6

ADC_MODE(ADC_VCC);

static const int MEASURE_INTERVAL_MILLIS = 60000;
char macAddress[18] = {0};

const char *ssid = "E7EA3E";
const char *password = "79296267";

//const char *mqttServer = "raspberrypi";
//const char *mqttUsername = "user";
//const char *mqttPassword = "yourpassword";
const char *mqttServer = "sergeypc";
const char *mqttUsername = "user";
const char *mqttPassword = "yourpassword";

char *mqttPubBase = (char *) "/devices";
char *mqttSubBase = (char *) "/control";

HX711 scale(PIN_DOUT, PIN_CLK);
float calibration_factor = -89070;
IControlCenter *center;
DhtTemperatureSensor temperatureSensor = DhtTemperatureSensor(PIN_TEMPERATURE, DHT22, (char *) "dht22");


uint32_t lastPublishTime = 0;

char *readMac(char *toRead) {
    uint8_t mac[6];
    wifi_get_macaddr(STATION_IF, mac);
    sprintf(toRead, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return toRead;
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT);
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.begin(115200);
    Serial.println("Start: ");

    Serial.print("ESP.getFreeSketchSpace(): ");
    Serial.println(ESP.getFreeSketchSpace());

    scale.set_scale(calibration_factor); //Adjust to this calibration factor
    center = new MqttControlCenter(readMac(macAddress), ssid, password, mqttServer, 1883, mqttUsername, mqttPassword);
    temperatureSensor.init();
}

void loop() {
    uint32_t currenttime = millis();
    if ((currenttime - lastPublishTime) > MEASURE_INTERVAL_MILLIS) {
//        uint32_t chipId = ESP.getChipId();
//        uint32_t flashChipId = ESP.getFlashChipId();
        float voltage = ESP.getVcc() / 1023.0f;
        float mass = scale.get_units(10);
        ITemperatureResult &temperatureResult = temperatureSensor.read();

        String pubStatus = String("Publish status: ");
        boolean status = center->publish(mqttPubBase,
                                         macAddress,
                                         const_cast<char *>("mass"),
                                         temperatureResult.getTemperature(),
                                         temperatureResult.getHumidity(),
                                         mass,
                                         voltage);
        pubStatus += String(status);
        boolean loopStatus = center->loop();
        Serial.println(pubStatus + String(", overall: ") + String(loopStatus));
        lastPublishTime = millis();
    } else {
        center->loop();
        int buttonStatus = digitalRead(D5);
        if (HIGH == buttonStatus) {
            Serial.print("Reset tare. ");
            Serial.println(buttonStatus);
            scale.tare(10);
        }
    }
}
