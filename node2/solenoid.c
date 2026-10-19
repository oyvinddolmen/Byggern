#include "solenoid.h"
#include "sam.h"
#include "time.h"
#include <stdbool.h>
#include <stddef.h>

#define SOLENIOD_PIN (1u << 26)  // Arduino Due D22 = PB26
#define PULSE_MS 50u

static bool was_pressed;

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

    // Node1 sends {x, y, button_state}: PRESSED = 0, NONE = 1.
    if (message->byte[2] > 1u) return;
    bool pressed = (message->byte[2] == 0u);

    if (pressed && !was_pressed) {
        PIOB->PIO_CODR = SOLENIOD_PIN;
        time_spinFor(msecs(PULSE_MS));
        PIOB->PIO_SODR = SOLENIOD_PIN;
    }

    // Require a release before another pulse.
    was_pressed = pressed;
}
