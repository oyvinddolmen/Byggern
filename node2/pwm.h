#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "can.h"

void controller_update(void);

// servo
void pwm_init(void);
void pwm_set_pulse_us(uint32_t pulse_us);
void pwm_pulse_from_controller(void);

// motor
void motor_init(void);
void motor_set_x(uint8_t x);
void motor_control_from_controller(void);

#endif