#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MENU_ACTION,
    MENU_SUBMENU,
    MENU_BACK
} MenuItemType;

typedef struct Menu Menu;

typedef struct {
    const char *name;
    MenuItemType type;

    union {
        void (*action)(void);
        Menu *submenu;
    };
} MenuItem;

struct Menu {
    const char *title;
    MenuItem *items;
    uint8_t item_count;
    Menu *parent;
};

// Menus defined in menu.c 
extern Menu main_menu;
extern Menu difficulty_menu;

// Current menu state
extern Menu *current_menu;
extern uint8_t selected_item_index;

// Menu control
void menu_init(void);
void menu_next_item(void);
void menu_previous_item(void);
void menu_select(void);
void update_display(void);

/* Game-over events must call menu_game_over() from the main loop. */
void menu_game_over(void);
bool menu_game_running(void);
bool menu_game_finished(void);
uint32_t menu_game_seconds(void);

// Menu actions 
void set_easy(void);
void set_hard(void);

#endif /* MENU_H */