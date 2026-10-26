#include "pid.h"
#include "time.h"

PID pid = { .kp = 1.0f, .ki = 0.0f, .kd = 0.0f, .integral = 0.0f, .previous_error = 0.0f , .dt = 0.01f}; // Initialize PID with default values and dt

static uint64_t last_update;
static float last_output;
static int update_initialized;


void pid_reset(void)
{
    pid.integral = 0.0f;
    pid.previous_error = 0.0f;
    last_update = 0;
    last_output = 0.0f;
    update_initialized = 0;
}

float pid_update(float target, float position)
{
    uint64_t now = time_now();
    uint64_t sample_period = msecs((uint64_t)(pid.dt * 1000.0f));

    /* The main loop may call this much faster than the 100 Hz controller.
     * Hold the previous output until a complete controller interval passes. */
    if (update_initialized && (now - last_update) < sample_period) {
        return last_output;
    }
    update_initialized = 1;
    last_update = now;

    float error = target - position;

    // Proportional term
    float proportional = pid.kp * error;

    // Integral term
    pid.integral += pid.ki * error * pid.dt;

    /* Bound the integral contribution to avoid windup during saturation. */
    if (pid.integral > 100.0f) pid.integral = 100.0f;
    if (pid.integral < -100.0f) pid.integral = -100.0f;

    // Derivative term
    float derivative = pid.kd * (error - pid.previous_error) / pid.dt;

    // Update previous error
    pid.previous_error = error;

    // Calculate the control output
    last_output = proportional + pid.integral + derivative;
    return last_output;
}
