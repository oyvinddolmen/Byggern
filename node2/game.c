#include <stdint.h>
#include <stdbool.h>
#include "game.h"
#include "diode.h"
#include "can.h"

static uint16_t score = 0;
static bool beam_blocked = false;

void score_update(uint16_t adc_value)
{
    if (!beam_blocked && adc_value < 800) {
        score++;
        beam_blocked = true;
    } else if (beam_blocked && adc_value > 900) {
        beam_blocked = false;
    }
}

uint16_t score_get(void)
{
    return score;
}

void score_reset(void)
{
    score = 0;
    // Keep beam state: resetting must not recount a blocked beam.
}

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
void send_score(void){
    CanMsg message;
    message.id = 0x13;
    message.length = 1;
    message.byte[0] = score_get();

    can_tx(message);
}

void run_game(void)
{
    score_reset();
    beam_blocked = false;
    diode_init(7); // Initialize ADC channel 7 (A0)


    while (1)
    {
        update_from_message();
        soleniod_update();

        score_update(adc_read());
        send_score();
    }
}