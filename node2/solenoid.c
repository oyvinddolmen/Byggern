#include "solenoid.h"
#include "sam.h"
#include "time.h"
#include <stdbool.h>
#include <stddef.h>

#define SOLENIOD_PIN (1u << 26)  // Arduino Due D22 = PB26
#define PULSE_MS 50u

static bool was_pressed;
static bool active;
static uint64_t pulse_end;

void soleniod_init(void)
{
    PMC->PMC_PCER0 = (1u << ID_PIOB);

    // MPSA56 PNP driver: HIGH = off, LOW = on.
    // Set the output latch before enabling the output.
    PIOB->PIO_SODR = SOLENIOD_PIN;
    PIOB->PIO_MDDR = SOLENIOD_PIN;
    PIOB->PIO_PER = SOLENIOD_PIN;
    PIOB->PIO_OER = SOLENIOD_PIN;

    was_pressed = false;
}

void soleniod_activate(CanMsg *message)
{
    if (message == NULL || message->length != 3) return;
    if (message->byte[2] > 1u) return;

    bool pressed = (message->byte[2] == 1u);

    if (pressed && !was_pressed && !active) {
        pulse_end = time_now() + msecs(PULSE_MS);
        active = true;
        PIOB->PIO_CODR = SOLENIOD_PIN;  // ON
    }

    was_pressed = pressed;
}

void soleniod_update(void)
{
    if (active && time_now() >= pulse_end) {
        PIOB->PIO_SODR = SOLENIOD_PIN;  // OFF
        active = false;
    }
}