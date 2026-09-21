#include <stdio.h>
#include "ioboard.h"
#include "oled.h"
#include "menu.h"
#include "clock.h"
#include "spi.h"
#include "ioboard.h"

void run_init(void);
void run_menu(void);

JoystickInput poll_joystick(void);
void update_display(void);
