#include <avr/io.h>
#include <stdio.h>

typedef enum {
    DISPLAY = 0,
    IOBOARD = 1,
    CAN = 2
} Slave;

void slave_select(Slave slave);
uint8_t transfer_byte(uint8_t data, Slave slave);
void spi_init(void);
