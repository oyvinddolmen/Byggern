#include "pid.h"
#include <stdio.h>

PID pid = { .kp = 1.5f, .ki = 0.0f, .kd = 0.0f, .integral = 0.0f, .previous_error = 0.0f , .dt = 0.01f}; // Initialize PID with default values and dt


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
    printf("PID output: %f\n", output); // Debugging: print the PID output
    return output;
}
