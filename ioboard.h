#include <avr/io.h>
#include <stdbool.h>

//measured values for the joystick
float JOYSTICK_X_NEGATIVE = 1.25;
float JOYSTICK_X_POSITIVE = 4.0;
float JOYSTICK_X_0 = 2.55;
float JOYSTICK_Y_NEGATIVE = 1.25;
float JOYSTICK_Y_POSITIVE = 4.0;
float JOYSTICK_Y_0 = 2.55;

//measured touch values
float TOUCH_X_NEGATIVE = 0.0;
float TOUCH_X_POSITIVE = 2.5;

float TOUCH_Y_NEGATIVE = 0.0;
float TOUCH_Y_POSITIVE = 2.5;


typedef struct {
    bool LEFT,
    bool RIGHT,
    bool UP,
    bool DOWN
} JoystickDirections;

typedef struct {
    int x;
    int y;
} Position;

void ioboard_init(void) {};

void auto_calibrate_joystick();
void auto_calibrate_touch();

JoystickDirections joystick_direction{};

Position joystick_position{};
Position touch_position{};

