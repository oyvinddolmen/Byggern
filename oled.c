#include "oled.h"
#include "spi.h"

void oled_init(void)
{
    // OLED initialization
}

void oled_reset(void)
{
    // OLED reset
}

void oled_home(void)
{
    // ...
}

void oled_goto_line(uint8_t line)
{
    // OLED-specific commands
}

void oled_goto_column(uint8_t column)
{
    // OLED-specific commands
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