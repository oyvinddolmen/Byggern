#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"

void uart_init(unsigned int ubrr) {
	// setter BAUD rate
	// lagre de 8 første bits i UBRR0High registeret. 8 siste i LOW
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	// enabler både receiver og transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1 << RXCIE0);
	sei();	// global interrupt enable
	
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);
}

void uart_send(unsigned char data) {
	// vent på tomt buffer. UDRE0 usart data register empty
	while( !(UCSR0A & (1 << UDRE0))) {
		// vent
	}
	UDR0 = data;
}

char uart_receive(void) {
	// vent på data
	while (!(UCSR0A & (1 << RXC0))) {
		// vent
	}
	
	return UDR0;
};