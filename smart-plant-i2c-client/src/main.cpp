#include <Arduino.h>
#include <Wire.h>

#define CAPACITIVE_SLAVE_ADDRESS 0x04

#define REG_GET_ERROR_STATUS                0x00
#define REG_GET_CAPACITANCE                 0x01
#define REG_GET_CAPACITANCE_IN_PROGRESS     0x02
#define REG_SET_ADDRESS                     0x03
#define REG_GET_ADDRESS                     0x04
#define REG_RESET                           0x05
#define REG_GET_VERSION                     0x06

#define ERROR_STATUS_OK                     0x30
#define ERROR_STATUS_WRONG_ADDRESS          0x31
#define ERROR_STATUS_WRONG_REGISTER         0x32

#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min
#define min(a,b) ((a)<(b)?(a):(b))

uint8_t caps[2];
uint8_t readI2CRegister(int addr, int reg, int byteNum, uint8_t result[]) {
    if (byteNum < sizeof(caps)) {
        return 0xff;
    }
    Wire.beginTransmission(addr);
    Wire.write(reg);
    uint8_t status = Wire.endTransmission();
    if (status == 0) {
        uint8_t size = Wire.requestFrom(addr, byteNum);
        for (int i = 0; i < min(size, byteNum); i++) {
            result[i] = Wire.read();
        }
    }
    return status;
}

inline uint16_t join(uint8_t *array, int startIdx) {
    return ((uint16_t)array[startIdx]) << 8 | array[startIdx + 1];
}

uint16_t getMoistureReading() {
    uint8_t status = readI2CRegister(CAPACITIVE_SLAVE_ADDRESS, REG_GET_CAPACITANCE, 2, caps);
    uint16_t cap = join(caps, 0);
    Serial.printf("status: %d, value: %d (high:%d | low:%d)\n", status, cap, caps[0], caps[1]);
    return cap;
}

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    Wire.begin(PIN_WIRE_SDA, PIN_WIRE_SCL);

    Serial.begin(115200);
    Serial.println("I2C Client");
}

void loop() {
    uint16_t ret = getMoistureReading();
    delay(2000);
}