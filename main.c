#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "usb_serial.h"

#define BAUD_RATE	1200

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define NULL (void *)0

typedef enum {
	PINSETA = 0,
	PINSETB = 1,
	PINSETC = 2,
	PINSETD = 3,
	PINSETE = 4,
	PINSETF = 5,
	PINSET_SENTINEL = 6
} pinset;

void setupPin(pinset set, int pin) {
	// First set all pins to read
	DDRA = 0;
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	DDRE = 0;
	DDRF = 0;
	
	// no pullups
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	PORTF = 0;
	
	// Now set the one pin to write, and pull it high
	switch (set) {
		case PINSETA: {
			DDRA |= pin;
			PORTA |= pin;
		} break;
  default:
			break;
	}
}

void testPin(pinset set, int pin) {
	setupPin(set, pin);
}

int main(void)
{
	CPU_PRESCALE(2);  // run at 4 MHz just to be super safe (3.3v compliant)

	usb_init();

	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);

	while (1) {
		// Wait for the USB serial connection, then cue the pintest
		if (usb_serial_available()) {
			for (int set = PINSETA; set < PINSET_SENTINEL; set++) {
				for (int pin = 1; pin < 0xff; pin <<= 1) {
					testPin(set, pin);
				}
			}
		}
	}
}
