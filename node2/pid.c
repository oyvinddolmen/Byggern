#include "pid.h"

static float dt = 0.01f; // Default time step in seconds.
static float integral = 0.0f;
static float derivative = 0.0f;
static float proportional = 0.0f;


void pid_reset(PID *pid)
{
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float pid_update(PID *pid, float target, float position)
{
    float error = target - position;

    // Proportional term
    proportional = pid->kp * error;

    // Integral term
    integral += pid->ki * error * dt;

    // Derivative term
    derivative = pid->kd * (error - pid->previous_error) / dt;

    // Update previous error
    pid->previous_error = error;

    // Calculate the control output
    float output = proportional + integral + derivative;


}
