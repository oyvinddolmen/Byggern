#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "pwm.h"
#include "encoder.h"
#include "uart.h"
#include "can.h"
#include "diode.h"
#include "game.h"
#include "motor.h"

/*
 * Remember to update the Makefile with the (relative) path to the uart.c file.
 * This starter code will not compile until the UART file has been included in the Makefile. 
 * If you get somewhat cryptic errors referencing functions such as _sbrk, 
 * _close_r, _write_r, _fstat etc, you have most likely not done that correctly.

 * If you get errors such as "arm-none-eabi-gcc: no such file", you may need to reinstall the arm gcc packages using
 * apt or your favorite package manager.
 */
#include "uart.h"
#include "can.h"
#include "diode.h"
#include "game.h"
#include "solenoid.h"

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

int main()
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS;
    pwm_init();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    uart_init(840000000, 96000);

    CanInit timing = {
        .phase2 = 3,
        .phase1 = 5,
        .propag = 4,
        .sjw    = 0,
        .brp    = 41,
        .smp    = 0
    };

    can_init(timing, 0);
    adc_init(7); // Initialize ADC channel 7 (A0)
    encoder_init();
    motor_init();
    motor_set_x(50);
    soleniod_init();


    while (1)
    {

        update_from_message();
        soleniod_update();

        score_update(adc_read());
        //printf("Score: %d\n", score_get());

        int32_t position = encoder_read();
        //printf("Encoder position: %ld\n", (long)position);

        //controller_update();    // sets motor and servo based on CAN messages
    }
}