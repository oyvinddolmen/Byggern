#define IOBOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool LEFT;
    bool RIGHT;
    bool UP;
    bool DOWN;
} JoystickDirections;

typedef struct {
    uint8_t x;
    uint8_t y;
} Position;

void ioboard_init(uint8_t joystick_x_0, uint8_t joystick_y_0);

void auto_calibrate_joystick(uint8_t x, uint8_t y);
void auto_calibrate_touch(uint8_t x, uint8_t y);

JoystickDirections joystick_direction(uint8_t x, uint8_t y);
Position joystick_position(uint8_t x, uint8_t y);
Position touch_position(uint8_t x, uint8_t y);
