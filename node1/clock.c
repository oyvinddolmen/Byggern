#include <avr/io.h>

// klokkesignal for ADCen
void clock_init(void)
{
    DDRD |= (1 << PD4);          // PD4/OC3A as output

    TCNT3 = 0;
    OCR3A = 1;                   // Compare value

    TCCR3A = (1 << COM3A0);      // Toggle OC3A on compare match
    TCCR3B = (1 << WGM32)        // CTC mode, OCR3A is TOP
            | (1 << CS30);       // Timer clock = CPU clock, no prescaler
}