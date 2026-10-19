#ifndef PID_H
#define PID_H

#include <stdint.h>

int16_t controller_update(uint8_t joystick_pct,
                          int32_t encoder_counts,
                          float dt);

void controller_reset(void);

#endif