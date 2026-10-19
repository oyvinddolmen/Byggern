#ifndef F_CPU
#define F_CPU 4915200UL
#endif
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
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
    game_timer_init();
    sei();
}

void update_display(void)
{
    oled_clear();
    oled_pos(0, 0);
    oled_print(current_menu->title);

    uint8_t first_item_line = 1;
    if (menu_game_running() || menu_game_finished()) {
        char text[21];
        snprintf(text, sizeof text, "Time: %lu s",
                 (unsigned long)menu_game_seconds());
        oled_pos(2, 0);
        oled_print(text);
        first_item_line = 4;
    }

    for (uint8_t i = 0; i < current_menu->item_count; i++) {
        oled_pos(i + first_item_line, 0);
        oled_print(i == selected_item_index ? "> " : "  ");
        oled_print(current_menu->items[i].name);
    }
}

/* One iteration, so CAN handling can run alongside the menu. */
void run_update(void)
{
    static Menu *displayed_menu;
    static uint8_t displayed_selection;
    static uint32_t displayed_seconds;

    JoystickInput input = poll_joystick();
    switch (input) {
        case UP: menu_previous_item(); break;
        case DOWN: menu_next_item(); break;
        case SELECT: menu_select(); break;
        default: break;
    }

    uint32_t seconds = menu_game_seconds();
    if (current_menu != displayed_menu ||
        selected_item_index != displayed_selection ||
        (menu_game_running() && seconds != displayed_seconds)) {
        update_display();
        displayed_menu = current_menu;
        displayed_selection = selected_item_index;
        displayed_seconds = seconds;
    }
}

void run_menu(void)
{
    while (1) {
        run_update();
        _delay_ms(10);
    }
}
