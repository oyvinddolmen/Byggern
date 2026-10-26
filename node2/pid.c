#include "pid.h"

void pid_reset(PID *pid)
{
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

float pid_update(PID *pid, float target, float position, float dt)
{
    
}
