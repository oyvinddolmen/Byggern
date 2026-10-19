#include "menu.h"
#include <stddef.h>
#include "clock.h"

Menu main_menu;
Menu difficulty_menu;

static uint32_t final_seconds;
static void start_game(void);
static void return_to_main_menu(void);

static Menu playing_menu = {"Playing", NULL, 0, NULL};
static MenuItem game_over_items[] = {
    {"Retry", MENU_ACTION, .action = start_game},
    {"Main Menu", MENU_ACTION, .action = return_to_main_menu}
};
static Menu game_over_menu = {
    "Game Over", game_over_items,
    sizeof(game_over_items) / sizeof(MenuItem), NULL
};

Menu *current_menu = &main_menu;
uint8_t selected_item_index = 0;

static MenuItem difficulty_items[] = {
    {"Easy", MENU_ACTION, .action = set_easy},
    {"Hard", MENU_ACTION, .action = set_hard},
    {"Back", MENU_BACK, .submenu = NULL}
    };

    Menu difficulty_menu = {
        "Difficulty",
        difficulty_items,
        sizeof(difficulty_items) / sizeof(MenuItem),
        &main_menu
    };

    //Main

static MenuItem main_items[] = {
        {"Start Game", MENU_ACTION, .action = start_game},
        {"Difficulty", MENU_SUBMENU, .submenu = &difficulty_menu}
    };

    Menu main_menu = {
        "Main Menu",
        main_items,
        sizeof(main_items) / sizeof(MenuItem),
        NULL
    };

void menu_init(void)
{
    current_menu = &main_menu;
    selected_item_index = 0;
}

void menu_next_item(void) {
    if (!current_menu || !current_menu->item_count) return;
    selected_item_index++;

    if (selected_item_index >= current_menu->item_count) {
        selected_item_index = 0;
    }
}

void menu_previous_item(void) {
    if (!current_menu || !current_menu->item_count) return;
    if (selected_item_index == 0) {
        selected_item_index = current_menu->item_count - 1;
    } else {
        selected_item_index--;
    }
}

void menu_select(void) {
    if (!current_menu || !current_menu->items ||
        selected_item_index >= current_menu->item_count) return;
    MenuItem *selected_item = &current_menu->items[selected_item_index];

    switch (selected_item->type) {
        case MENU_ACTION:
            if (selected_item->action != NULL) {
                selected_item->action();
            }
            break;
        case MENU_SUBMENU:
            if (selected_item->submenu != NULL) {
                current_menu = selected_item->submenu;
                selected_item_index = 0;
            }
            break;
        case MENU_BACK:
            if (current_menu->parent != NULL) {
                current_menu = current_menu->parent;
                selected_item_index = 0;
            }
            break;
    }
}

void set_easy(void) {
    // Set game difficulty to easy
}
void set_hard(void) {
    // Set game difficulty to hard
}

static void start_game(void)
{
    game_timer_reset();
    final_seconds = 0;
    current_menu = &playing_menu;
    selected_item_index = 0;
}

static void return_to_main_menu(void)
{
    menu_init();
}

bool menu_game_running(void)
{
    return current_menu == &playing_menu;
}

bool menu_game_finished(void)
{
    return current_menu == &game_over_menu;
}

uint32_t menu_game_seconds(void)
{
    return menu_game_running() ? game_timer_seconds() : final_seconds;
}

/* Call from the main loop when a game-over event is received. */
void menu_game_over(void)
{    
    if (!menu_game_running()) return;
    final_seconds = game_timer_seconds();
    current_menu = &game_over_menu;
    selected_item_index = 0;
}
