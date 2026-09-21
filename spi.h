#ifndef SPI_H
#define SPI_H
#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>

typedef enum {
    NONE = -1,
    DISPLAY = 0,
    IOBOARD = 1,
    CAN = 2
} Slave;

void spi_slave_select(Slave slave);
uint8_t spi_transfer_byte(uint8_t data, Slave slave);
void spi_init(void);

#endif
