#include "ioboard.h"

// Dead zone measured in ADC counts. Adjust as needed.
#define JOYSTICK_DEADZONE 4

typedef struct {
    uint8_t min;
    uint8_t max;
} AxisRange;

static uint8_t joystick_x_center;
static uint8_t joystick_y_center;

static AxisRange joystick_x_range = {UINT8_MAX, 0};
static AxisRange joystick_y_range = {UINT8_MAX, 0};
static AxisRange touch_x_range = {UINT8_MAX, 0};
static AxisRange touch_y_range = {UINT8_MAX, 0};

static void update_range(AxisRange *range, uint8_t value)
{
    // Separate checks let the first sample set both endpoints.
    if (value < range->min) {
        range->min = value;
    }

    if (value > range->max) {
        range->max = value;
    }
}

static uint8_t position_percent(uint8_t value, AxisRange range)
{
    // No usable calibration yet.
    if (range.max <= range.min) {
        return 0;
    }

    if (value <= range.min) {
        return 0;
    }

    if (value >= range.max) {
        return 100;
    }

    return (uint8_t)(
        ((uint16_t)(value - range.min) * 100u)
        / (range.max - range.min)
    );
}

void ioboard_init()
{   volatile char *adc = (char *) 0x1400; // Start address for the ADC
    *adc = 0x00; // Select ADC channel 0 (X joystick)

    uint8_t joystick_x_0 = *adc; // Read ADC value for X joystick
    uint8_t joystick_y_0 = *adc; // Read ADC value for Y joystick
    joystick_x_center = joystick_x_0;
    joystick_y_center = joystick_y_0;

    joystick_x_range = (AxisRange){joystick_x_0, joystick_x_0};
    joystick_y_range = (AxisRange){joystick_y_0, joystick_y_0};

    touch_x_range = (AxisRange){UINT8_MAX, 0};
    touch_y_range = (AxisRange){UINT8_MAX, 0};
}

void auto_calibrate_joystick(uint8_t x, uint8_t y)
{
    update_range(&joystick_x_range, x);
    update_range(&joystick_y_range, y);
}

void auto_calibrate_touch(uint8_t x, uint8_t y)
{
    update_range(&touch_x_range, x);
    update_range(&touch_y_range, y);
}

JoystickDirections joystick_direction(uint8_t x, uint8_t y)
{
    // Signed differences allow negative displacement.
    int16_t dx = (int16_t)x - joystick_x_center;
    int16_t dy = (int16_t)y - joystick_y_center;

    JoystickDirections direction = {
        .LEFT  = dx < -JOYSTICK_DEADZONE,
        .RIGHT = dx >  JOYSTICK_DEADZONE,
        .UP    = dy > -JOYSTICK_DEADZONE,
        .DOWN  = dy <  JOYSTICK_DEADZONE
    };

    return direction;
}

Position joystick_position(uint8_t x, uint8_t y)
{
    Position position = {
        .x = position_percent(x, joystick_x_range),
        .y = position_percent(y, joystick_y_range)
    };
    auto_calibrate_joystick(x,y);
    
    return position;
}

Position touch_position(uint8_t x, uint8_t y)
{
    Position position = {
        .x = position_percent(x, touch_x_range),
        .y = position_percent(y, touch_y_range)
    };
    auto_calibrate_touch(x,y);

    return position;
}


void print_joystick_position(uint8_t x, uint8_t y)
{
    Position pos = joystick_position(x, y);
    printf("Joystick position: X=%3d%%, Y=%3d%%\n", pos.x, pos.y);
}
void print_touch_position(uint8_t x, uint8_t y)
{
    Position pos = touch_position(x, y);
    printf("Touch position: X=%3d%%, Y=%3d%%\n", pos.x, pos.y);
}


void adc_read_all(uint8_t channels[4]){

    volatile char *adc = (char *) 0x1400; // Start address for the ADC
    *adc = 0x00; // Select ADC channel 0 (X joystick)


    //Henter neste kanal hver gang du henter read
    channels[0] = *adc; // Read ADC value for X joystick
    channels[1] = *adc; // Read ADC value for Y joystick
    channels[3] = *adc; // Read ADC value for X touch
    channels[2] = *adc; // Read ADC value for Y touch
}

void adc_print_all(uint8_t channels[4]){
    printf("ADC values: X joystick=%3d, Y joystick=%3d, X touch=%3d, Y touch=%3d\n",
           channels[0], channels[1], channels[2], channels[3]);
    printf("Joystick position: X=%3d%%, Y=%3d%%\n", joystick_position(channels[0], channels[1]).x, joystick_position(channels[0], channels[1]).y);
    printf("Touch position: X=%3d%%, Y=%3d%%\n", touch_position(channels[2], channels[3]).x, touch_position(channels[2], channels[3]).y);
    printf("Joystick direction: LEFT=%d, RIGHT=%d, UP=%d, DOWN=%d\n",
          joystick_direction(channels[0], channels[1]).LEFT,
          joystick_direction(channels[0], channels[1]).RIGHT,
          joystick_direction(channels[0], channels[1]).UP,
          joystick_direction(channels[0], channels[1]).DOWN);
}