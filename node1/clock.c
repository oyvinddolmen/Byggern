#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "clock.h"

#ifndef F_CPU
#define F_CPU 4915200UL
#endif

static volatile uint32_t elapsed_seconds;

/* Timer1 counts seconds; Timer3 remains the ADC clock. */
void game_timer_init(void)
{
    TCCR1B = 0;
    TCCR1A = 0;
    TCNT1 = 0;
    OCR1A = F_CPU / 256UL - 1;
    elapsed_seconds = 0;
    TIFR = (1 << OCF1A);
    TIMSK |= (1 << OCIE1A);
    TCCR1B = (1 << WGM12) | (1 << CS12);
}

ISR(TIMER1_COMPA_vect)
{
    elapsed_seconds++;
}

void game_timer_reset(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCNT1 = 0;
        TIFR = (1 << OCF1A);
        elapsed_seconds = 0;
    }
}

uint32_t game_timer_seconds(void)
{
    uint32_t seconds;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        seconds = elapsed_seconds;
    }
    return seconds;
}


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
