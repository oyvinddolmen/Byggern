#include "PID.h"

// CALIBRATION:
// Place racket at LEFT position before encoder_init().
// Measure encoder count at RIGHT position and set below.
// Can be negative. Zero disables motor regulation.
#define ENCODER_FULL_TRAVEL_COUNTS 1600

// +1 if motor_set(>50) moves racket to the right.
// Otherwise use -1.
#define MOTOR_SIGN_FOR_RIGHT 1

// Controller gains
#define KP 0.6f
#define KI 0.0f  // Start with P, then try 0.1f

#define MAX_PWM_PERCENT 25.0f
#define INTEGRAL_LIMIT 250.0f
#define POSITION_TOLERANCE 1.0f
#define REFERENCE_MARGIN 10.0f

static float integral = 0.0f;

static float clampf(float x, float low, float high)
{
    if (x < low) return low;
    if (x > high) return high;
    return x;
}

void controller_reset(void)
{
    integral = 0.0f;
}

int16_t controller_update(uint8_t joystick_pct,
                          int32_t encoder_counts,
                          float dt)
{
    // No calibration? Keep motor stopped.
    if (ENCODER_FULL_TRAVEL_COUNTS == 0 || dt <= 0.0f) {
        controller_reset();
        return 50;
    }

    // Convert encoder count to position in percent.
    float position = 100.0f * (float)encoder_counts /
                     (float)ENCODER_FULL_TRAVEL_COUNTS;

    position = clampf(position, 0.0f, 100.0f);

    // Joystick determines desired position.
    // Keep a 10% margin from mechanical endpoints.
    float reference = clampf((float)joystick_pct,
                             REFERENCE_MARGIN,
                             100.0f - REFERENCE_MARGIN);

    // Position error
    float error = reference - position;

    // Stop very close to reference.
    if (error > -POSITION_TOLERANCE &&
        error < POSITION_TOLERANCE) {
        controller_reset();
        return 50;
    }

    // Integral update
    float next_integral = clampf(
        integral + error * dt,
        -INTEGRAL_LIMIT,
        INTEGRAL_LIMIT
    );

    float next_u = KP * error + KI * next_integral;

    // Simple anti-windup
    if (!((next_u > MAX_PWM_PERCENT && error > 0.0f) ||
          (next_u < -MAX_PWM_PERCENT && error < 0.0f))) {
        integral = next_integral;
    }

    // PI output: signed PWM percentage
    float u = KP * error + KI * integral;

    u = clampf(u, -MAX_PWM_PERCENT, MAX_PWM_PERCENT);

    // Stop commands that would drive beyond endpoints.
    if ((position <= 0.0f && u < 0.0f) ||
        (position >= 100.0f && u > 0.0f)) {
        controller_reset();
        return 50;
    }

    // motor_set(0..100) has neutral at 50.
    // Therefore halve the signed PWM percentage.
    float delta = MOTOR_SIGN_FOR_RIGHT * u * 0.5f;

    int32_t motor_command = 50 +
        (int32_t)(delta + (delta >= 0 ? 0.5f : -0.5f));

    return (int16_t)motor_command;
}

