#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

#include "usb_serial.h"

#define BAUD_RATE	1200

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

int main(void)
{
	CPU_PRESCALE(2);  // run at 4 MHz just to be super safe (3.3v compliant)

	usb_init();

	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);

	while (1) {
		// Wait for the USB serial connection, then cue the pintest
		if (usb_serial_available()) {
			
		}
	}
}
