#ifndef PID_H
#define PID_H

typedef struct {
    float kp, ki, kd;
    float integral;
    float previous_error;
    float dt; // Time step in seconds
} PID;

void pid_reset(void);

/* target and position use the same units; dt is seconds.
 * Returns -50..50 for motor_set_x(50 + output).
 */
float pid_update(float target, float position);

#endif
