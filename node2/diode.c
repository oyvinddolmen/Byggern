#include <sam.h>
#include "diode.h"


//adc_init(7) gives A0
void adc_init(uint8_t channel)
{
    if (channel > 14) return;  // Channel 15 is the temperature sensor

    // ADC peripheral ID is 37: enable bit 37 - 32 in PCER1.
    PMC->PMC_PCER1 = (1u << (ID_ADC - 32));

    ADC->ADC_CR = ADC_CR_SWRST;    // resets adc

    ADC->ADC_MR =
        ADC_MR_PRESCAL(41) |       // 84 MHz / (2 * 42) = 1 MHz
        ADC_MR_STARTUP_SUT64 |     // 64 ADC clock cycles startup time
        ADC_MR_TRACKTIM(15) |       // tracking time
        ADC_MR_TRANSFER(1);         // transfer period

    ADC->ADC_CHDR = 0xFFFFu;       // Disable all channels
    ADC->ADC_CHER = (1u << channel);
}

uint16_t adc_read(void)
{
    ADC->ADC_CR = ADC_CR_START;           // Start conversion

    while (!(ADC->ADC_ISR & (1u << 7)));  // Wait for channel 7

    return ADC->ADC_CDR[7];               // Read result; clears EOC7
}