#include "Arduino.h"
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN PB0

int main2(void) {
    DDRB |= (1 << LED_PIN);
    while(true) {
        PORTB |= (1 << LED_PIN);
        _delay_ms(500);
        PORTB &= ~(1 << LED_PIN);
        _delay_ms(500);
    }
    return 0;
}