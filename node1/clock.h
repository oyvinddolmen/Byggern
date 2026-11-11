#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init(void);
void game_timer_init(void);
void game_timer_reset(void);
uint32_t game_timer_seconds(void);

#endif
