#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

// Initialize PB12 / D20 as motor PWM (1 kHz) and PC23 / D7 as direction.
void motor_init(void);

// Signed command from -100 to +100. Magnitude is PWM duty cycle (%)
// and sign chooses direction. Zero gives 0% PWM.
void motor_set_x(uint8_t x);

#endif