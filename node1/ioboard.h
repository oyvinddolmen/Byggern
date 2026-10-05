#ifndef IOBOARD_H
#define IOBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "spi.h"

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

typedef enum {
    X_joystick = 0,
    Y_joystick = 1,
    X_touch = 2,
    Y_touch = 3
} Variable;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SELECT,
    JOYSTICK_NONE
} JoystickInput;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t btn;
} JoystickData;


void ioboard_init(void);

void auto_calibrate_joystick(uint8_t x, uint8_t y);
void auto_calibrate_touch(uint8_t x, uint8_t y);

JoystickDirections joystick_direction(uint8_t x, uint8_t y);
Position joystick_position(uint8_t x, uint8_t y);
Position touch_position(uint8_t x, uint8_t y);
JoystickData joystick_read(void);

void print_joystick_position(uint8_t x, uint8_t y);
void print_touch_position(uint8_t x, uint8_t y);

/* Call about every 10 ms; three equal samples debounce transitions. */
JoystickInput poll_joystick(void);

void adc_read_all(uint8_t channels[4]);
void adc_print_all(uint8_t channels[4]);

#endif
