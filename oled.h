#include <avr/io.h>
#include <stdio.h>

oled_init();
oled_reset();
oled_home();
oled_goto_line(uint8_t line);
oled_goto_column(uint8_t column);
oled_clear_line(uint8_t line);
oled_pos(uint8_t row, uint8_t column);
oled_print(char* str);  