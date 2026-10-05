
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

void send_joystick_pos(void){
		JoystickData data = joystick_read();
	Position joystick_pos = joystick_position(data.x, data.y);


	CanMessage message = {
		.id = 0x123,
		.length = 2,
		.data = {joystick_pos.x, joystick_pos.y}
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
	
	can_init();
	can_test();

	while(1){
		send_joystick_pos();
	}

}

