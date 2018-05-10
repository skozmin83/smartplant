#include "TinyWireS.h"                  // wrapper class for I2C slave routines
#include <avr/io.h>
#include <util/delay.h>
#include <USBAPI.h>

#define I2C_SLAVE_ADDR 0x04             // I2C slave address (46, can be changed)

#define PB3 3

// Global Variables
uint8_t command[4] = {0};    // Command received from master
int commandLength = 0;      // Command number of bytes

// Gets called when the ATtiny receives an I2C write slave request
void receiveEvent(uint8_t commandbytes) {
    // save the number of bytes sent from the master
    commandLength = commandbytes;
    // store the data from the master into the data buffer
    for (int i = 0; i < commandLength; i++) {
        command[i] = TinyWireS.receive();
    }
}

// Gets called when the ATtiny receives an I2C read slave request
void requestEvent() {
    switch (command[0]) {
        case 'a': {
            PORTB |= (1 << PB3);
            break;
        }
        case 'd': {
            PORTB &= ~(1 << PB3);
            break;
        }
    }
    TinyWireS.send(command[0]);
}

int main() {
    sei();
    // initialize the TinyWireS and usiTwiSlave libraries
    TinyWireS.begin(I2C_SLAVE_ADDR);      // init I2C Slave mode
    // register the onReceive() callback function
    TinyWireS.onReceive(receiveEvent);
    // register the onRequest() callback function
    TinyWireS.onRequest(requestEvent);

    DDRB |= (1 << PB3);
    while (1) {
//        PORTB |= (1 << PB3);
//        _delay_ms(50);
//        PORTB &= ~(1 << PB3);
//        _delay_ms(1000);
    }
}


