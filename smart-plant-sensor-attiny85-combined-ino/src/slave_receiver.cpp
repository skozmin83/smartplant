

#include <TinyWire.h>
#include <Arduino.h>
#define led_pin PB1

byte own_address = 0x04;

/*
I2C Slave Receive Callback:
Note that this function is called from an interrupt routine and shouldn't take long to execute
*/
void onI2CReceive(int howMany){
	// loops, until all received bytes are read
	while(TinyWire.available()>0){
		// toggles the led everytime, when an 'a' is received
		if(TinyWire.read()=='a') digitalWrite(led_pin, !digitalRead(led_pin));
	}
}

void onI2CRequest() {
	// sends one byte with content 'b' to the master, regardless how many bytes he expects
	// if the buffer is empty, but the master is still requesting, the slave aborts the communication
	// (so it is not blocking)
	TinyWire.send('b');
	TinyWire.send('c');
}

void setup2() {
	// config led_pin as Output for driving an LED
//	pinMode(led_pin, OUTPUT);
	
	// config TinyWire library for I2C slave functionality
	TinyWire.begin( own_address );
	// sets callback for the event of a slave receive
	TinyWire.onReceive( onI2CReceive );
	TinyWire.onRequest( onI2CRequest ); 
}

void loop2() {
    // fade in from min to max in increments of 5 points:
    for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
        // sets the value (range from 0 to 255):
        analogWrite(led_pin, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
    }

    // fade out from max to min in increments of 5 points:
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
        // sets the value (range from 0 to 255):
        analogWrite(led_pin, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
    }
}

int main(void) {
    init();
	sei();
	setup2();
	while(true) {
	    loop2();
	};
	return 0;
}
