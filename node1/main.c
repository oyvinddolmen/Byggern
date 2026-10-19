
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

	print_joystick_position(pos);

	ButtonState button_state = button_read((Button){.right = true, .left = false, .number = 5});

	CanMessage message = {
		.id = 0x12,
		.length = 3,
		.data = {pos.x, pos.y, button_state}
	};

	can_send(&message);
}



int main() {
    uart_init(31);
	SRAM_init(); 
	clock_init();
	ioboard_init();

	uint8_t channels[4];

	spi_init();
	oled_reset();
	_delay_ms(10);
    oled_init();
	menu_init();
	
	can_init();
	can_test();
	soleniod_init();

	while(1){
		send_iodata();
	}

}

