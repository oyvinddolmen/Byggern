#include "sam.h"
#include "motor.h"

// TTK4155 motor shield:
// PB12 / Arduino D20 = PWMH0 (peripheral B) -> A3959 ENABLE
// PC23 / Arduino D7 = GPIO -> A3959 PHASE
#define MOTOR_PWM_PIN (1u << 12)
#define MOTOR_DIR_PIN (1u << 23)
#define MOTOR_PWM_CHANNEL 0u
#define MOTOR_PWM_MASK (1u << MOTOR_PWM_CHANNEL)

// MCK = 84 MHz, PWM clock = MCK/8 = 10.5 MHz.
// 10,500 PWM ticks per period -> 1 kHz.
#define MOTOR_PWM_PERIOD 10500u

void motor_init(void)
{
    // Enable PIOB, PIOC and PWM peripheral clocks.
    // PCER registers are write-one-to-enable: other clocks stay enabled.
    PMC->PMC_PCER0 = (1u << ID_PIOB) | (1u << ID_PIOC);
    PMC->PMC_PCER1 = (1u << (ID_PWM - 32u));

    // Start with direction LOW and enable PIOC GPIO output on PC23.
    PIOC->PIO_CODR = MOTOR_DIR_PIN;
    PIOC->PIO_PER = MOTOR_DIR_PIN;
    PIOC->PIO_OER = MOTOR_DIR_PIN;

    // Only disable PWM channel 0; channel 1 (servo) is unaffected.
    PWM->PWM_DIS = MOTOR_PWM_MASK;
    while (PWM->PWM_SR & MOTOR_PWM_MASK) {
    }

    // PB12 -> peripheral B (PWMH0) rather than normal GPIO.
    PIOB->PIO_ABSR |= MOTOR_PWM_PIN;
    PIOB->PIO_PDR = MOTOR_PWM_PIN;

    // Channel 0: MCK/8, left aligned, HIGH at the start of a pulse.
    // Do not touch PWM_CLK: channel 1 uses clock A for the servo.
    PWM->PWM_CH_NUM[0].PWM_CMR =
    PWM_CMR_CPRE_MCK_DIV_8 | PWM_CMR_CPOL;

    PWM->PWM_CH_NUM[0].PWM_CPRD = MOTOR_PWM_PERIOD;
    PWM->PWM_CH_NUM[0].PWM_CDTY = 0u; // Motor off by default

    PWM->PWM_ENA = MOTOR_PWM_MASK;
}


void motor_set_x(uint8_t x)
{
    int8_t cmd = x;

    // Clamp input to 0 ... 100.
    if (cmd > 100) cmd = 100;
    if (cmd < 0) cmd = 0;

    // Convert 0...100 to -100...100.
    // 0 = full reverse, 50 = stop, 100 = full forward.
    cmd = (cmd - 50) * 2;

    // Set motor direction.
    if (cmd < 0) {
    PIOC->PIO_SODR = MOTOR_DIR_PIN; // PHASE high
    cmd = -cmd;
    } else {
    PIOC->PIO_CODR = MOTOR_DIR_PIN; // PHASE low
    }

    // Convert 0...100% to PWM duty cycle.
    uint32_t duty =
    ((uint32_t)cmd * MOTOR_PWM_PERIOD) / 100u;

    // Update PWM.
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = duty;
}
