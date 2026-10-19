#ifndef PID_H
#define PID_H

#include <stdint.h>

// Reset PID states (e.g. at startup or CAN timeout).
void pid_reset(void);

// joystick_x: target position from Node 1 (0..100 %).
// encoder_counts: measured position relative to the known LEFT endpoint.
// dt: seconds since previous controller update.
// Return: 0..100 command for existing motor_set_x(); 50 = stop.
uint8_t pid_update(uint8_t joystick_x, int32_t encoder_counts, float dt);

#endif
