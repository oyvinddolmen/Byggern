#include <avr/io.h>
#include <stdio.h>

typedef enum {
    DISPLAY = 0,
    IOBOARD = 1,
    CAN = 2
} Slave;

void slave_select(Slave slave);

