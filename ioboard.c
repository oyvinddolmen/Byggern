#include <avr/io.h>
#include "ioboard.h"

void ioboard_init(float joystick_x_0, float joystick_y_0){
    // Initialize the joystick and touch positions
    JOYSTICK_X_0 = joystick_x_0;
    JOYSTICK_Y_0 = joystick_y_0;
}

void auto_calibrate_joystick(float x, float y) {
    // Implement auto-calibration logic for the joystick here
    if (x < JOYSTICK_X_NEGATIVE) {
        JOYSTICK_X_NEGATIVE = x;
    } else if (x > JOYSTICK_X_POSITIVE) {
        JOYSTICK_X_POSITIVE = x;
    }
    if (y < JOYSTICK_Y_NEGATIVE) {
        JOYSTICK_Y_NEGATIVE = y;
    } else if (y > JOYSTICK_Y_POSITIVE) {
        JOYSTICK_Y_POSITIVE = y;
    }
}

void auto_calibrate_touch(float x, float y) {
    // Implement auto-calibration logic for the touch here
    if (x < touch_x_negative) {
        touch_x_negative = x;
    }else if (x > touch_x_positive) {
        touch_x_positive = x;
    }
    if (y < touch_y_negative) {
        touch_y_negative = y;
    }else if (y > touch_y_positive) {
        touch_y_positive = y;
    }
}

JoystickDirections joystick_direction(float x, float y) {
    JoystickDirections direction = {false, false, false, false};
    if (x  < JOYSTICK_X_0){
        direction.LEFT = true;
    } else if (x > JOYSTICK_X_0) {
        direction.RIGHT = true;
    }
    if (y < JOYSTICK_Y_0) {
        direction.UP = true;
    } else if (y > JOYSTICK_Y_0) {
        direction.DOWN = true;
    }
    return direction;
}

Position joystick_position(float x, float y) {
    Position position = {0, 0};
    position.x = (int)((x - JOYSTICK_X_NEGATIVE) / (JOYSTICK_X_POSITIVE - JOYSTICK_X_NEGATIVE) * 100);
    position.y = (int)((y - JOYSTICK_Y_NEGATIVE) / (JOYSTICK_Y_POSITIVE - JOYSTICK_Y_NEGATIVE) * 100);
    return position;
}

Position touch_position(float x, float y) {
    Position position = {0, 0};
    position.x = (int)((x - TOUCH_X_NEGATIVE) / (TOUCH_X_POSITIVE - TOUCH_X_NEGATIVE) * 100);
    position.y = (int)((y - TOUCH_Y_NEGATIVE) / (TOUCH_Y_POSITIVE - TOUCH_Y_NEGATIVE) * 100);
    return position;
}