#include <stdint.h>
#include <stdbool.h>
#include "game.h"
#include "diode.h"
#include "can.h"
#include "solenoid.h"
#include "pwm.h"

static bool beam_blocked = false;


void update_from_message(void){
    CanMsg message;
    bool received = can_rx(&message);
    if (received){
        if (message.id == 0x12 && message.length == 3){
            soleniod_activate(&message);
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


    while (1)
    {
        update_from_message();
        soleniod_update();

        check_game_over(adc_read());
    }
}