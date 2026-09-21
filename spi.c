#include <spi.h>
#include <avr/io.h>

void spi_init(void)
{
    // Outputs:
    // PB7 = SCK
    // PB5 = MOSI
    // PB4 = hardware SS, must be output
    // PB3 = OLED D/C
    // PB2 = OLED RESET
    // PB1 = DISP_CS
    // PB0 = IO_CS
    DDRB |= (1 << PB7)
          | (1 << PB5)
          | (1 << PB4)
          | (1 << PB3)
          | (1 << PB2)
          | (1 << PB1)
          | (1 << PB0);

    // PB6 = MISO input
    DDRB &= ~(1 << PB6);

    // Hardware SS: keep HIGH
    PORTB |= (1 << PB4);

    // Deselect both SPI slaves
    // Active LOW -> HIGH means not selected
    PORTB |= (1 << PB1);   // DISP_!CS
    PORTB |= (1 << PB0);   // IO_!CS

    // OLED reset inactive
    // Active LOW
    PORTB |= (1 << PB2);

    // D/C LOW initially = command mode
    PORTB &= ~(1 << PB3);

    // Enable SPI as Master
    // Clock = F_CPU / 16
    // Mode 0: CPOL = 0, CPHA = 0
    // MSB first
    SPCR = (1 << SPE)
         | (1 << MSTR)
         | (1 << SPR0);
}

void slave_select(Slave slave)
{
    // Deselect all slaves
    PORTB |= (1 << PB4) | (1 << PB1) | (1 << PB0);


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

uint8_t transfer_byte(uint8_t data, Slave slave)
{
    slave_select(slave); // Select the appropriate slave
    SPDR = data; // Load data into the SPI Data Register
    while (!(SPSR & (1 << SPIF))); // Wait for transmission to complete

    return SPDR; // Return the received data
}
