#include <spi.h>


void slave_select(Slave slave)
{
    // Set all slave select pins high (deselect all slaves)
    PORTB |= (1 << PB2) | (1 << PB1) | (1 << PB0);

    // Select the desired slave by setting its pin low
    switch (slave) {
        case DISPLAY:
            PORTB &= ~(1 << PB1); // Select display
            break;
        case IOBOARD:
            PORTB &= ~(1 << PB0); // Select ioboard
            break;
        case CAN:
            PORTB &= ~(1 << PB4); // Select CAN
            break;
        default:
            // Invalid slave, do nothing
            break;
    }
}