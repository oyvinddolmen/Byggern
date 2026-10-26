#include <stdint.h>


static float time_constant = 0.1f; // Time constant for the low-pass filter

uint16_t adc_lowpass(uint16_t sample)
{
    static float filtered;
    static uint8_t initialized = 0;

    if (!initialized) {
        filtered = sample;
        initialized = 1;
    }

    filtered += time_constant * ((float)sample - filtered);

    return (uint16_t)(filtered + 0.5f); // Round to nearest integer
}


uint16_t adc_median(uint16_t sample)
{
    static uint16_t samples[5];
    static uint8_t index;
    static uint8_t initialized;

    if (!initialized) {
        for (uint8_t i = 0; i < 5; i++)
            samples[i] = sample;
        initialized = 1;
    }

    samples[index] = sample;
    index = (index + 1) % 5;

    uint16_t sorted[5];
    for (uint8_t i = 0; i < 5; i++)
        sorted[i] = samples[i];

    for (uint8_t i = 1; i < 5; i++) {
        uint16_t value = sorted[i];
        uint8_t j = i;

        while (j > 0 && sorted[j - 1] > value) {
            sorted[j] = sorted[j - 1];
            j--;
        }
        sorted[j] = value;
    }

    return sorted[2];
}