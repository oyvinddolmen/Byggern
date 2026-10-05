#ifndef F_CPU
#define F_CPU 4915200UL
#endif
#include <util/delay.h>
#include "run.h"
#include "ioboard.h"
#include "menu.h"
#include "oled.h"
#include "clock.h"

void run_init(void)
{
    clock_init();
	ioboard_init();
    oled_reset();
    oled_init();
    menu_init();
}

void update_display(void)
{
    oled_clear();
    oled_pos(0, 0);
    oled_print(current_menu->title);

    for (uint8_t i = 0; i < current_menu->item_count; i++) {
        oled_pos(i + 1, 0); // Start from line 1
        if (i == selected_item_index) {
            oled_print("> "); // Indicate selection
        } else {
            oled_print("  "); // No selection
        }
        oled_print(current_menu->items[i].name);
    }
}

void run_menu(void)
{
    update_display();
    while (1) {
        JoystickInput input = poll_joystick();
        if (input != JOYSTICK_NONE) {
            switch (input) {
                case UP:
                    menu_previous_item();
                    break;
                case DOWN:
                    menu_next_item();
                    break;
                case SELECT:
                    menu_select();
                    break;
                default:
                    break;
            }
            update_display();
        }
        _delay_ms(10);
    }
}
