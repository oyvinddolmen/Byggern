#include "run.h"

void run_init(void)
{
    oled_init();
    menu_init();
    clock_init();
	ioboard_init();
    spi_init();
}

JoystickInput poll_joystick(void)
{
    return read_joystick();
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
    while (1) {
        if (input = poll_joystick() != NONE) {
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
        sleep_ms(400); // Delay for human input, in ms
    }
}
