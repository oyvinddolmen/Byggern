
#define F_CPU 4915200UL		// processor speed 4.915 MHz
#include "uart.h"
#include "sram_test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"
#include "ioboard.h"
#include "spi.h"
#include "oled.h"

int main() {
    uart_init(31);
	SRAM_init(); 
	clock_init();
	ioboard_init();

	uint8_t channels[4];

	spi_init();
	oled_reset();
	_delay_ms(10);
    oled_init();

	
    
	oled_print("KUK");

    

    while (1){
	}
	return 0;
}

