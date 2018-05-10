#include "Arduino.h"
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN 0
#define LED_GND_PIN 1

int main(void) {
    DDRA |= (1 << LED_PIN);
    DDRA |= (1 << LED_GND_PIN);
    PORTA &= ~(1 << LED_GND_PIN);
    while(true) {
        PORTA |= (1 << LED_PIN);
        _delay_ms(1000);
        PORTA &= ~(1 << LED_PIN);
        _delay_ms(1000);
    }
    return 0;
}