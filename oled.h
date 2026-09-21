#include <avr/io.h>
#include <stdio.h>
#define F_CPU 4915200UL		// processor speed 4.915 MHz
#include <util/delay.h>

void oled_init();
void oled_reset();
void oled_home();
void oled_write_command(uint8_t command);
void oled_write_data(uint8_t data);
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);
void oled_clear_line(uint8_t line);
void oled_clear();
void oled_pos(uint8_t row, uint8_t column);
void oled_write_char(char c);
void oled_print(const char *text);