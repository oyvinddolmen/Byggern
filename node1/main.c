
#define F_CPU 4915200UL		// processor speed 4.915 MHz
#include "uart.h"
#include "sram_test.h"
#include <avr/io.h>
#include <util/delay.h>
#include "clock.h"
#include "ioboard.h"
#include "spi.h"
#include "oled.h"
#include "menu.h"
#include "run.h"
#include "can.h"

void send_iodata(void){
    uint8_t channels[4];
    adc_read_all(channels);

    Position pos = joystick_position(channels[0], channels[1]);



	ButtonState button_state = button_read((Button){.right = true, .left = false, .number = 5});

	CanMessage message = {
		.id = 0x12,
		.length = 3,
		.data = {pos.x, pos.y, button_state}
	};

	can_send(&message);
}


int main(void)
{
    uart_init(31);
    SRAM_init();
    spi_init();
    run_init();
    bool can_ready = can_init();

    while (1) {
        run_update();
        if (can_ready && menu_game_running()) {
            send_iodata();
        }

        CanMessage message;
        if (can_ready && can_receive(&message) &&
            message.id == 0x13 &&
            message.length == 1 &&
            message.data[0] == 0xFF) {
            menu_game_over();
            
            CanMessage game_over_message = {
                .id = 0x12,
                .length = 3,
                .data = {50, 50, BTN_NONE}
            };
            can_send(&game_over_message);
        }

        _delay_ms(10);

    }
}
