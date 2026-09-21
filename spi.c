#include <spi.h>


void slave_select(Slave slave)
{
    // Deselect all slaves
    PORTB |= (1 << PB2) | (1 << PB1) | (1 << PB0);


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

void write_byte(uint8_t data)
{
    SPDR = data; // Load data into the SPI Data Register
    while (!(SPSR & (1 << SPIF))); // Wait for transmission to complete
}

uint8_t read_byte(void)
{
    SPDR = 0xFF; // Send dummy data to initiate SPI read
    while (!(SPSR & (1 << SPIF))); // Wait for reception to complete
    return SPDR; // Return the received data
}