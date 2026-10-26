#include <stdint.h>
#include <stdbool.h>
#include "game.h"
#include "diode.h"
#include "can.h"
#include "solenoid.h"
#include "pwm.h"
#include <stdio.h>
#include "motor.h"

static bool beam_blocked = false;


void update_from_message(float *target_position){
    CanMsg message;
    bool received = can_rx(&message);
    if (received){
        if (message.id == 0x12 && message.length == 3){
            solenoid_activate(&message);
            *target_position = (float)message.byte[0];
            pwm_pulse_from_controller(&message);
        }
    }
}
void send_game_over(void){
    CanMsg message;
    message.id = 0x13;
    message.length = 1;
    message.byte[0] = 0xFF;

    can_tx(message);
}

void check_game_over(uint16_t adc_value)
{   
    printf("ADC Value: %u\n", adc_value);
    
    if (!beam_blocked && adc_value < 800) {
        send_game_over();
        beam_blocked = true;
    } else if (beam_blocked && adc_value > 900) {
        beam_blocked = false;
    }
}

void run_game(void)
{
    beam_blocked = false;
    MotorLimits limits = motor_find_limits();
    if (!limits.valid) {
        printf("Error: Could not find motor limits.\n");
        return;
    }
    float target_position = 50.0f; // Default target position (center)

    while (1)
    {
        
        update_from_message(&target_position);
        soleniod_update();
        motor_set_x_pid((uint8_t)target_position, limits.left, limits.right);
        

        check_game_over(adc_read());
    }
}