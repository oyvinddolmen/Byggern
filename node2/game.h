#ifndef GAME_H
#define GAME_H

#include <stdint.h>

void update_from_message(void);
void send_game_over(void);
void check_game_over(uint16_t adc_value);
void run_game(void);

#endif