#ifndef FILTERING_H
#define FILTERING_H

#include <stdint.h>

uint16_t adc_lowpass(uint16_t sample);
uint16_t adc_median(uint16_t sample);

#endif
