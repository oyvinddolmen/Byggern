#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

/* A3959: PB12 (PWMH0, peripheral B) = ENABLE; PC23 = PHASE.
 * Pin mapping follows this project's existing pwm.c.
 */
void motor_init(void);

/* Node 1 joystick X: 0..100, centre = 50.
 * 45..55 stops; deflection controls PWM duty, sign controls PHASE.
 * Call after motor_init(). This function does not receive CAN messages.
 */
void motor_set_x(uint8_t x);


#endif
