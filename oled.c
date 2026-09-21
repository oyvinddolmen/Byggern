#include "oled.h"
#include "spi.h"
#include "fonts.h"


void oled_init(void)
{
    // fra 
    oled_write_command(0xae); // display off
    oled_write_command(0xa1); //segment remap
    oled_write_command(0xda); //common pads hardware: alternative
    oled_write_command(0x12);
    oled_write_command(0xc8); //common output scan direction:com63~com0
    oled_write_command(0xa8); //multiplex ration mode:63
    oled_write_command(0x3f);
    oled_write_command(0xd5); //display divide ratio/osc. freq. mode
    oled_write_command(0x80);
    oled_write_command(0x81); //contrast control
    oled_write_command(0x50);
    oled_write_command(0xd9); //set pre-charge period
    oled_write_command(0x21);
    oled_write_command(0x20); //Set Memory Addressing Mode
    oled_write_command(0x02); //As page adressing mode
    oled_write_command(0xdb); //VCOM deselect level mode
    oled_write_command(0x30);
    oled_write_command(0xad); //master configuration
    oled_write_command(0x00);
    oled_write_command(0xa4); //out follows RAM content
    oled_write_command(0xa6); //set normal display
    oled_write_command(0xaf); // display on
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
    PORTB &= ~(1 << PB3);    // D/C# = 1: command
    spi_slave_select(DISPLAY); // Select the appropriate slave
    spi_transfer_byte(command, DISPLAY);
    spi_slave_select(NONE); 
}

void oled_write_data(uint8_t data)
{
    PORTB |= (1 << PB3);     // D/C# = 0: display data
    spi_slave_select(DISPLAY); // Select the appropriate slave
    spi_transfer_byte(data, DISPLAY);
    spi_slave_select(NONE);
}

void oled_goto_line(uint8_t line)
{
    if (line > 7) {
        return; // Invalid line number
    }
    oled_write_command(0xB0 | line); // Set page address
}

void oled_goto_column(uint8_t column)
{
    if (column > 127) {
        return; // Invalid column number
    }
     // Bits 0–3 of the column address
    oled_write_command(0x00 | (column & 0x0F));

    // Bits 4–7 of the column address
    oled_write_command(0x10 | ((column >> 4) & 0x0F));
}


void oled_clear_line(uint8_t line)
{
    if (line > 7)
    {
        return;
    }

    // Start at beginning of requested page
    oled_goto_line(line);
    oled_goto_column(0);

    // One byte for each of the 128 columns
    for (uint8_t column = 0; column < 128; column++)
    {
        oled_write_data(0x00);
    }
}

void oled_clear()
{
    for (uint8_t line = 0; line < 8; line++)
    {
        oled_clear_line(line);
    }

    // Put the RAM pointer back at top-left
    oled_pos(0,0);
}

void oled_pos(uint8_t row, uint8_t column)
{
    oled_goto_line(row);
    oled_goto_column(column);
}

void oled_write_char(char c)
{
    // Font table contains printable ASCII characters:
    // ASCII 32 (' ') to ASCII 126 ('~')
    if (c < 32 || c > 126)
    {
        c = '?';
    }

    // Convert ASCII code to font table index.
    // Example:
    // ' ' = ASCII 32 -> index 0
    // 'A' = ASCII 65 -> index 33
    uint8_t index = c - 32;

    // font5 contains 5 columns for each character
    for (uint8_t column = 0; column < 5; column++)
    {
        // Font is stored in Flash/PROGMEM,
        // so we must use pgm_read_byte()
        uint8_t data =
            pgm_read_byte(&font5[index][column]);

        // Send one vertical column of pixels
        oled_write_data(data);
    }

    // Add one blank column between characters
    oled_write_data(0x00);
}

void oled_print(const char *text)
{
    while (*text != '\0')
    {
        oled_write_char(*text);

        // Move pointer to next character
        text++;
    }
}