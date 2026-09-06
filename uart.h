#ifndef UART_H
#define UART_H

void uart_init(unsigned int ubrr);
void uart_send(unsigned char c);
char uart_receive(void);

#endif