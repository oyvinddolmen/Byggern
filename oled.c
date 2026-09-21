#include "oled.h"

oled_init();
oled_reset();
oled_home();
oled_goto_line(line);
oled_goto_column(column);
oled_clear_line(line);
oled_pos(row,column);
oled_print(char*);