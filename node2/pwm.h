#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "can.h"

void pwm_init(void);
void pwm_set_pulse_us(uint32_t pulse_us);
void pwm_pulse_from_controller(CanMsg *message);

#endif