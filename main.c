#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "usb_serial.h"

#define STRING_ANNOUNCE_PREAMBLE	"Testing pin "
#define STRING_ANNOUNCE_POSTSCRIPT	"... "

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
		case PINSETB: {
			DDRB |= pin;
			PORTB |= pin;
		} break;
		case PINSETC: {
			DDRC |= pin;
			PORTC |= pin;
		} break;
		case PINSETD: {
			DDRD |= pin;
			PORTD |= pin;
		} break;
		case PINSETE: {
			DDRE |= pin;
			PORTE |= pin;
		} break;
		case PINSETF: {
			DDRF |= pin;
			PORTF |= pin;
		} break;
	default:
			break;
	}
}

char charForPinset(pinset set) {
	if (set >= 0 && set <= PINSET_SENTINEL) {
		return 'A' + set;
	}
	return '?';
}

char charForPinMask(int pin) {
	for (int i = 0; i < 8; i++) {
		if (pin == (1 << i)) {
			return '0' + i;
		}
	}
	return '?';
}

void printPin(pinset set, int pin) {
	usb_serial_putchar(charForPinset(set));
	usb_serial_putchar(charForPinMask(pin));
	usb_serial_putchar(' ');
}

void announcePin(pinset set, int pin) {
	usb_serial_write((const uint8_t *)STRING_ANNOUNCE_PREAMBLE, sizeof(STRING_ANNOUNCE_PREAMBLE));
	printPin(set, pin);
	usb_serial_write((const uint8_t *)STRING_ANNOUNCE_POSTSCRIPT, sizeof(STRING_ANNOUNCE_POSTSCRIPT));
}

int checkPin(pinset set, int pin) {
	switch (set) {
		case PINSETA:
			return PINA & pin;
		case PINSETB:
			return PINB & pin;
		case PINSETC:
			return PINC & pin;
		case PINSETD:
			return PIND & pin;
		case PINSETE:
			return PINE & pin;
		case PINSETF:
			return PINF & pin;
		default:
			return 0;
	}
	return 0;
}

void testPin(pinset set, int pin) {
	setupPin(set, pin);
	announcePin(set, pin);
	
	// test each other pin for shorts, and print the ones that connect
	for (pinset tSet = 0; tSet < PINSET_SENTINEL; tSet++) {
		for (int tPin = 0; tPin < 8; tPin++) {
			if (checkPin(tSet, 1 << tPin)) {
				printPin(tSet, 1 << tPin);
			}
		}
	}
	
	usb_serial_putchar('\r');
	usb_serial_putchar('\n');
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
				for (int pin = 0; pin < 8; pin++) {
					testPin(set, 1 << pin);
				}
			}
			break;
		}
	}
}
