#include "TinyWireS.h"
#include <avr/io.h>
#include <util/delay.h>
#include <USBAPI.h>

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#define I2C_SLAVE_ADDR                      0x04             // default I2C slave address
#define FIRMWARE_VERSION                    0x01

#define LED                                 PB0
#define LED_DDR                             DDRB
#define LED_PORT                            PORTB
#define CHANNEL_CAPACITANCE                 PB4

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

uint8_t readRegister[4] = {0};    // Command received from master
int commandLength = 0;      // Command number of bytes
volatile uint8_t adcInProgress = 0;
uint8_t capMeasurementInProgress = 0;
uint8_t newAddress = I2C_SLAVE_ADDR;
volatile uint16_t capacitance = 0;
uint8_t error = 0;

#define isCapacitanceMeasurementInProgress() capMeasurementInProgress
#define reset() wdt_enable(); while(1) {}

inline static void ledSetup() {
    LED_DDR |= _BV(LED);
    LED_PORT &= ~_BV(LED);
}

inline static void ledOn() {
    LED_PORT |= _BV(LED);
}

inline static void ledOff() {
    LED_PORT &= ~_BV(LED);
}

inline static void ledToggle() {
    LED_PORT ^= _BV(LED);
}

inline static void adcSetup() {
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA |= _BV(ADIE);
    ADMUX = 0;
}

inline static void sleepWhileADC() {
    adcInProgress = 1;
    while (adcInProgress) {
        set_sleep_mode(SLEEP_MODE_ADC);
        sleep_mode();
    }
}

ISR(ADC_vect) {
    adcInProgress = 0;
    //nothing, just wake up
}

uint16_t adcReadChannel(uint8_t channel) {
    ADMUX = channel;
    ADCSRA |= _BV(ADSC);
    sleepWhileADC();
    // loop_until_bit_is_clear(ADCSRA, ADSC);
    uint16_t ret = ADC;
    return ret;
}

uint16_t adcReadChannelSync(uint8_t channel) {
    ADMUX = channel;
    ADCSRB |= _BV(ADSC);

    while (ADCSRB & _BV(ADSC));  // wait for it to finish
    uint16_t ret = ADC;
    return ret;
}

uint16_t getCapacitance() {
    capMeasurementInProgress = 1;
    capacitance = adcReadChannel(CHANNEL_CAPACITANCE);
    capMeasurementInProgress = 0;
    return capacitance;
}

static int samples = 10;
uint64_t duration;
uint16_t getSyncCapacitance() {
    duration = 0;
    uint32_t cur = 0;
    for (int i = 0; i < samples; i++) {
        int counter = 0;
        do {
            cur = pulseIn(CHANNEL_CAPACITANCE, HIGH);
        } while (cur != 0 && counter++ < 10);
        duration += cur;
    }
    return static_cast<uint16_t>(duration / samples);
//    return pulseIn(CHANNEL_CAPACITANCE, HIGH);
//    uint16_t cap = adcReadChannel(CHANNEL_CAPACITANCE);
//    uint16_t cap = analogRead(CHANNEL_CAPACITANCE);
//    analogWrite(LED, cap);
//    return cap;
}

inline static void wdt_disable() {
    MCUSR = 0; // The MCU Status Register provides information on which reset source caused an MCU Reset
    WDTCR &= ~_BV(WDE);
}

inline static void wdt_enable() {
    WDTCR = _BV(WDE);
}

static inline void setupPowerSaving() {
//    PRR = _BV(PRTIM0) | _BV(PRTIM1) | _BV(PRTIM2) | _BV(PRSPI) | _BV(PRUSART0) | _BV(PRUSART1); //shut down everything we don't use
//    ACSR = _BV(ACD); //disable comparators
//    DIDR0 = _BV(ADC3D) | _BV(ADC5D) | _BV(ADC7D);//disable input buffers for analog pins
}

int getSlaveAddress() {
//    uint8_t address = eeprom_read_byte(I2C_ADDRESS_EEPROM_LOCATION);
//    if (!twiIsValidAddress(address)) {
//        address = I2C_SLAVE_ADDR;
//    }
    return I2C_SLAVE_ADDR;
}

// Gets called when the ATtiny receives an I2C write slave request
void receiveEvent(uint8_t incomingBytesNum) {
    // save the number of bytes sent from the master
    commandLength = incomingBytesNum;
    // store the data from the master into the data buffer
    for (int i = 0; i < commandLength; i++) {
        readRegister[i] = TinyWireS.receive();
    }
}

// Gets called when the ATtiny receives an I2C read slave request
void requestEvent() {
    error = ERROR_STATUS_OK;
    switch (readRegister[0]) {
        case REG_GET_ERROR_STATUS: {
            TinyWireS.send(error);
            break;
        }
        case REG_GET_CAPACITANCE: {
            int16_t localCap = capacitance != 0 ? capacitance : 0xffff;
            TinyWireS.send(localCap >> 8);
            TinyWireS.send(localCap & 0x00FF);
//            capacitance = getCapacitance();
            break;
        }
        case REG_GET_CAPACITANCE_IN_PROGRESS: {
            TinyWireS.send(isCapacitanceMeasurementInProgress());
            break;
        }
        case REG_SET_ADDRESS:
            newAddress = TinyWireS.receive();
            if (newAddress > 127 || newAddress <= 0) {
                error = ERROR_STATUS_WRONG_ADDRESS;
            } else {
                eeprom_write_byte((uint8_t *) 0x01, newAddress);
            }
            break;
        case REG_GET_ADDRESS:
            TinyWireS.send(eeprom_read_byte((uint8_t *) 0x01));
            break;
        case REG_RESET: {
            reset();
            break;
        }
        case REG_GET_VERSION:
            TinyWireS.send(FIRMWARE_VERSION);
            break;
        default:
            error = ERROR_STATUS_WRONG_REGISTER;
    }
}

void loop2() {
    capacitance = getSyncCapacitance();
//    ledOn();
    _delay_ms(1000);
//    ledOff();
}

void setup2() {
//    wdt_disable();
    setupPowerSaving();
    ledSetup();
    pinMode(CHANNEL_CAPACITANCE, INPUT);

    adcSetup();

    ledOn();
    _delay_ms(1000);
    ledOff();

    // initialize the TinyWireS and usiTwiSlave libraries
    TinyWireS.begin(getSlaveAddress());      // init I2C Slave mode
    // register the onReceive() callback function
    TinyWireS.onReceive(receiveEvent);
    // register the onRequest() callback function
    TinyWireS.onRequest(requestEvent);
}

int main() {
    init();
    sei();
    setup2();
    while (1) {
////        checkSensors();
//        PORTB |= (1 << PB1);
//        _delay_ms(50);
//        PORTB &= ~(1 << PB1);
//        _delay_ms(1000);
        loop2();
    }
//    int ledPin = PB1;
//
//    while (true) {
//        // fade in from min to max in increments of 5 points:
//        for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
//            // sets the value (range from 0 to 255):
//            analogWrite(ledPin, fadeValue);
//            // wait for 30 milliseconds to see the dimming effect
//            delay(30);
//        }
//
//        // fade out from max to min in increments of 5 points:
//        for (int fadeValue = 255; fadeValue >= 0; fadeValue -= 5) {
//            // sets the value (range from 0 to 255):
//            analogWrite(ledPin, fadeValue);
//            // wait for 30 milliseconds to see the dimming effect
//            delay(30);
//        }
//    }
}
