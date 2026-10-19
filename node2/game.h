#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>

void score_update(uint16_t adc_value);
uint16_t score_get(void);
void score_reset(void);

void run_game(void);

#endif