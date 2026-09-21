/*
 * byggern.c
 *
 * Created: 31.08.2026 08:43:04
 * Author : oyvind
 */ 

#define F_CPU 4915200UL		// processor speed 4.915 MHz
#include "uart.h"
#include "sram_test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"
#include "ioboard.h"
#include "spi.h"

int main(void)
{
    uart_init(31);
	SRAM_init(); 
	clock_init();
	ioboard_init();

	uint8_t channels[4];

	spi_init();
    oled_init();

    // Start at PAGE0, column 0
    oled_pos(0, 0);

    // Test text
    oled_print("HELLO");

    while (1)
    {
    }
}

