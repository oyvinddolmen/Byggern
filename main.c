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

int main(void)
{
    uart_init(31);
	SRAM_init(); 
	SRAM_test(); 
	
}

