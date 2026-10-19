#include "PID.h"
#include <stdbool.h>

// IMPORTANT: Verify this number by moving the racket manually
// from the known LEFT endpoint to the RIGHT endpoint.
// It may be negative if encoder counts decrease towards the right.
#define ENCODER_FULL_TRAVEL_COUNTS 1600

// Set to +1 when motor_set_x(>50) moves towards the right;
// otherwise set to -1. Verify at low power before enabling PID.
#define MOTOR_SIGN_FOR_RIGHT -1

// Start with P control. Increase KI later if friction creates an offset.
// KD is normally unnecessary for this lab (encoder differentiation is noisy).
#define PID_KP 1.0f
#define PID_KI 0.0f
#define PID_KD 0.0f

// Max change from motor_set_x(50), in command points.
// Note that motor.c additionally applies its own deadzone and 0.3 scaling.
#define MAX_COMMAND_OFFSET 30.0f
#define INTEGRAL_LIMIT 150.0f
#define POSITION_TOLERANCE 1.0f
#define REFERENCE_MARGIN 10.0f

static float integral = 0.0f;
static float prev_position = 0.0f;
static bool have_prev_position = false;

static float limit(float value, float low, float high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

void pid_reset(void)
{
    integral = 0.0f;
    prev_position = 0.0f;
    have_prev_position = false;
}

uint8_t pid_update(uint8_t joystick_x, int32_t encoder_counts, float dt)
{
    if (ENCODER_FULL_TRAVEL_COUNTS == 0 || dt <= 0.0f || dt > 0.25f) {
        pid_reset();
        return 50u;
    }

    // Convert encoder position to percent of calibrated travel.
    // Do NOT clamp measured position: out-of-range measurements are
    // important for detecting an attempted move beyond an endpoint.
    float position = (100.0f * (float)encoder_counts) /
                     (float)ENCODER_FULL_TRAVEL_COUNTS;

    // Joystick directly supplies 0..100; no scaling is needed.
    float reference = limit((float)joystick_x,
                            REFERENCE_MARGIN, 100.0f - REFERENCE_MARGIN);
    float error = reference - position;

    // Derivative on measurement avoids a large kick on reference changes.
    float d_position = 0.0f;
    if (have_prev_position) {
        d_position = (position - prev_position) / dt;
    }
    prev_position = position;
    have_prev_position = true;

    if (error > -POSITION_TOLERANCE && error < POSITION_TOLERANCE) {
        integral = 0.0f;
        return 50u;
    }

    // Candidate integral and output for simple conditional anti-windup.
    float candidate_integral = limit(integral + error * dt,
                                     -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
    float candidate_u = PID_KP * error + PID_KI * candidate_integral
                        - PID_KD * d_position;

    if (!((candidate_u > MAX_COMMAND_OFFSET && error > 0.0f) ||
          (candidate_u < -MAX_COMMAND_OFFSET && error < 0.0f))) {
        integral = candidate_integral;
    }

    float u = PID_KP * error + PID_KI * integral - PID_KD * d_position;
    u = limit(u, -MAX_COMMAND_OFFSET, MAX_COMMAND_OFFSET);

    // Software endpoint protection (only valid with verified calibration).
    if ((position <= 0.0f && u < 0.0f) ||
        (position >= 100.0f && u > 0.0f)) {
        pid_reset();
        return 50u;
    }

    // motor_set_x() expects 0..100, center 50 = stop.
    float offset = (float)MOTOR_SIGN_FOR_RIGHT * u;
    int32_t command = 50 + (int32_t)(offset + (offset >= 0.0f ? 0.5f : -0.5f));
    if (command < 0) command = 0;
    if (command > 100) command = 100;
    return (uint8_t)command;
}
