#include "oled.h"
#include "spi.h"


void oled_init(void)
{
    // OLED initialization
}

void oled_reset(void)
{
    PORTB &= ~(1 << PB2); // Set RESET LOW
    _delay_ms(10);        // Wait for 10 ms
    PORTB |= (1 << PB2);  // Set RESET HIGH
}

void oled_home(void)
{
    // ...
}

void oled_write_command(uint8_t command)
{
    PORTB &= ~(1 << PB3);    // D/C# = 0: command
    slave_select(DISPLAY);   // OLED CS = 0

    spi_transfer_byte(command);

    slave_select(NONE);  // OLED CS = 1
}

void oled_write_data(uint8_t data)
{
    PORTB |= (1 << PB3);     // D/C = 1: display data
    slave_select(DISPLAY);

    spi_transfer_byte(data);

    slave_select(NONE);
}

void oled_goto_line(uint8_t line)
{
    if line > 7 {
        return; // Invalid line number
    }
    oled_write_command(0xB0 | line); // Set page address
}

void oled_goto_column(uint8_t column)
{
    if column > 127 {
        return; // Invalid column number
    }
     // Bits 0–3 of the column address
    oled_write_command(0x00 | (column & 0x0F));

    // Bits 4–7 of the column address
    oled_write_command(0x10 | ((column >> 4) & 0x0F));
}


void oled_clear_line(uint8_t line)
{
    // ...
}

void oled_pos(uint8_t row, uint8_t column)
{
    oled_goto_line(row);
    oled_goto_column(column);
}

void oled_print(char *str)
{
    while (*str) {
        // Send character to OLED
        str++;
    }
}
