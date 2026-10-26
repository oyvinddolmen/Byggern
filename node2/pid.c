#include "pid.h"

PID pid = { .kp = 50.0f, .ki = 5.0f, .kd = 25.0f, .integral = 0.0f, .previous_error = 0.0f , .dt = 1f}; // Initialize PID with default values and dt


void pid_reset(void)
{
    pid.integral = 0.0f;
    pid.previous_error = 0.0f;
}

float pid_update(float target, float position)
{
    float error = target - position;

    // Proportional term
    float proportional = pid.kp * error;

    // Integral term
    pid.integral += pid.ki * error * pid.dt;

    // Derivative term
    float derivative = pid.kd * (error - pid.previous_error) / pid.dt;

    // Update previous error
    pid.previous_error = error;

    // Calculate the control output
    float output = proportional + pid.integral + derivative;

    return output;
}
