#ifndef PWM_H
#define PWM_H

#include "can.h"
#include <stdint.h>

// servo
void pwm_init(void);
void pwm_set_pulse_us(uint32_t pulse_us);
void controller_update(void);
void pwm_pulse_from_controller(CanMsg *message);

#endif