#include "motor.h"
#include "sam.h"

#define MOTOR_CHANNEL     0u
#define MOTOR_CHANNEL_MASK (1u << MOTOR_CHANNEL)
#define MOTOR_ENABLE_PIN  (1u << 12)  /* PB12, PWMH0 */
#define MOTOR_PHASE_PIN   (1u << 23)  /* PC23, GPIO */
#define MOTOR_PWM_PERIOD  525u        /* 84 MHz / 8 / 525 = 20 kHz */
#define MOTOR_CENTRE      50
#define MOTOR_DEADZONE    5
#define MOTOR_REVERSE     0           /* Set to 1 to reverse joystick direction. */

void motor_init(void)
{
    PMC->PMC_PCER0 = (1u << ID_PIOB) | (1u << ID_PIOC);
    PMC->PMC_PCER1 = (1u << (ID_PWM - 32u));

    /* Hold ENABLE low while setting up the channel. */
    PIOB->PIO_CODR = MOTOR_ENABLE_PIN;
    PIOB->PIO_MDDR = MOTOR_ENABLE_PIN;
    PIOB->PIO_OER = MOTOR_ENABLE_PIN;
    PIOB->PIO_PER = MOTOR_ENABLE_PIN;

    PIOC->PIO_CODR = MOTOR_PHASE_PIN;
    PIOC->PIO_MDDR = MOTOR_PHASE_PIN;
    PIOC->PIO_OER = MOTOR_PHASE_PIN;
    PIOC->PIO_PER = MOTOR_PHASE_PIN;

    PWM->PWM_DIS = MOTOR_CHANNEL_MASK;
    while (PWM->PWM_SR & MOTOR_CHANNEL_MASK) {
    }

    /* Use MCK/8 directly: do not change PWM_CLK used by servo channel 1.
     * CPOL=1 makes ENABLE high for CDTY ticks: zero duty means stopped.
     */
    PWM->PWM_CH_NUM[MOTOR_CHANNEL].PWM_CMR =
        PWM_CMR_CPRE_MCK_DIV_8 | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[MOTOR_CHANNEL].PWM_CPRD = MOTOR_PWM_PERIOD;
    PWM->PWM_CH_NUM[MOTOR_CHANNEL].PWM_CDTY = 0u;

    PIOB->PIO_ABSR |= MOTOR_ENABLE_PIN;
    PWM->PWM_ENA = MOTOR_CHANNEL_MASK;
    PIOB->PIO_PDR = MOTOR_ENABLE_PIN;
}

void motor_set_x(uint8_t x)
{
    if (x > 100u) {
        x = 100u;
    }

    int16_t offset = (int16_t)x - MOTOR_CENTRE;
    if (MOTOR_REVERSE) {
        offset = -offset;
    }

    uint32_t magnitude = (uint32_t)(offset < 0 ? -offset : offset);
    uint32_t duty = 0u;

    if (magnitude > MOTOR_DEADZONE) {
        if (offset > 0) {
            PIOC->PIO_SODR = MOTOR_PHASE_PIN;
        } else {
            PIOC->PIO_CODR = MOTOR_PHASE_PIN;
        }

        /* Rescale the remaining travel to 0..100% duty. */
        duty = (magnitude - MOTOR_DEADZONE) * MOTOR_PWM_PERIOD /
               (MOTOR_CENTRE - MOTOR_DEADZONE);
    }
    duty = duty*0.3;

    /* Applied at the next PWM period boundary, within 50 us. */
    PWM->PWM_CH_NUM[MOTOR_CHANNEL].PWM_CDTYUPD = duty;
}
