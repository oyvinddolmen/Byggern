#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "sam.h"
#include "uart.h"
#include "can.h"
#include "pwm.h"
#include "motor.h"
#include "encoder.h"
#include "PID.h"
#include "diode.h"
#include "game.h"
#include "solenoid.h"
#include "time.h"

#define CONTROL_PERIOD_MS 10u
#define CAN_TIMEOUT_MS 250u
#define PRINT_PERIOD_MS 250u

int main(void)
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS;

    uart_init(84000000u, 9600u);

    motor_init();
    motor_set_x(50);  // Stop, even before first CAN message.
    pwm_init();

    CanInit timing = {
        .phase2 = 3,
        .phase1 = 5,
        .propag = 4,
        .sjw = 0,
        .brp = 41,
        .smp = 0
    };
    can_init(timing, 0);

    adc_init(7);
    encoder_init();
    soleniod_init();
    pid_reset();

    uint8_t joystick_x = 50u;
    bool have_can = false;
    uint64_t last_can = time_now();
    uint64_t last_control = time_now();
    uint64_t last_print = time_now();

    printf("Node 2: PID position control ready\n");

    while (1)
    {
        CanMsg message;
        uint64_t now = time_now();

        // A single CAN read supplies BOTH servo and position reference.
        if (can_rx(&message)) {
            if (message.id == 0x12u && message.length == 3u) {
                joystick_x = message.byte[0];
                uint8_t joystick_y = message.byte[1];

                if (joystick_x > 100u) joystick_x = 100u;
                if (joystick_y > 100u) joystick_y = 100u;

                // Servo stays on joystick Y (900..2100 us).
                pwm_set_pulse_us(900u + 12u * (uint32_t)joystick_y);

                // Existing short, edge-triggered solenoid shot.
                soleniod_activate(&message);

                have_can = true;
                last_can = now;
            }
        }

        soleniod_update();
        score_update(adc_read());

        // Control step approximately every 10 ms; dt is measured, not guessed.
        now = time_now();
        if ((now - last_control) >= msecs(CONTROL_PERIOD_MS)) {
            float dt = totalSeconds(now - last_control);
            last_control = now;

            if (!have_can || (now - last_can) > msecs(CAN_TIMEOUT_MS)) {
                pid_reset();
                motor_set_x(50); // CAN timeout or no first command
            } else {
                int32_t position = encoder_read();
                uint8_t command = pid_update(joystick_x, position, dt);
                motor_set_x(command);
            }
        }

        if ((now - last_print) >= msecs(PRINT_PERIOD_MS)) {
            last_print = now;
            printf("Encoder: %ld, reference: %u, score: %u\n",
                   (long)encoder_read(), (unsigned)joystick_x,
                   (unsigned)score_get());
        }
    }
}
