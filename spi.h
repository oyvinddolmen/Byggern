#include <avr/io.h>
#include <stdio.h>

typedef enum {
    DISPLAY = 0,
    IOBOARD = 1,
    CAN = 2
} Slave;

void slave_select(Slave slave);
void write_byte(uint8_t data);
uint8_t read_byte(void);
void spi_init(void);
