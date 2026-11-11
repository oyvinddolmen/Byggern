#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void encoder_init(void);
int32_t encoder_read(void);
int32_t encoder_read_position(int32_t left_limit, int32_t right_limit);

#endif