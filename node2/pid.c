#include "pid.h"

void pid_reset(PID *pid)
{
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float pid_update(PID *pid, float target, float position, float dt)
{
    if (dt <= 0.0f) return 0.0f;

    float error = target - position;
    float integral = pid->integral + pid->ki * error * dt;
    float derivative = (error - pid->previous_error) / dt;
    float output = pid->kp * error + integral + pid->kd * derivative;

    // Prevent integral windup; allow it to unwind during saturation.
    if ((output >= -50.0f && output <= 50.0f) ||
        (output > 50.0f && error < 0.0f) ||
        (output < -50.0f && error > 0.0f)) {
        pid->integral = integral;
    }

    pid->previous_error = error;

    if (output > 50.0f) output = 50.0f;
    if (output < -50.0f) output = -50.0f;
    return output;
}
