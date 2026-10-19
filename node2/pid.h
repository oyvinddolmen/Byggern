#ifndef PID_H
#define PID_H

typedef struct {
    float kp, ki, kd;
    float integral;
    float previous_error;
} PID;

void pid_reset(PID *pid);

/* target and position use the same units; dt is seconds.
 * Returns -50..50 for motor_set_x(50 + output).
 */
float pid_update(PID *pid, float target, float position, float dt);

#endif
