#ifndef UART_H
#define UART_H
#include <stdio.h>

void uart_init(unsigned int ubrr);
int uart_send(char c, FILE *stream);
char uart_receive(FILE *stream);

#endif