#include "sam.h"
#include "pwm.h"

#define SERVO_PWM_CHANNEL  1u
#define SERVO_PWM_MASK     (1u << SERVO_PWM_CHANNEL)
#define SERVO_PIN_MASK     (1u << 13)

void pwm_init(void)
{
    // 1. Aktiver periferiklokkene til PIOB og PWM.
    PMC->PMC_PCER0 = (1u << ID_PIOB);
    PMC->PMC_PCER1 = (1u << (ID_PWM - 32u));

    // 2. Deaktiver PWM-kanal 1 under konfigureringen.
    PWM->PWM_DIS = SERVO_PWM_MASK;
    while (PWM->PWM_SR & SERVO_PWM_MASK) {
    }

    // 3. Velg periferifunksjon B for PB13.
    PIOB->PIO_ABSR |= SERVO_PIN_MASK;

    // 4. Overlat PB13 til periferien.
    PIOB->PIO_PDR = SERVO_PIN_MASK;

    // 5. PWM-klokke A: 84 MHz / 84 = 1 MHz.
    PWM->PWM_CLK = PWM_CLK_PREA(0)
                 | PWM_CLK_DIVA(84);

    // 6. Bruk klokke A, venstrejustering og høy startpuls.
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CMR =
        PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // 7. Periode: 20 000 us = 20 ms.
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CPRD = 20000u;

    // 8. Høy puls: 1 500 us = 1,5 ms.
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTY = 1500u;

    // 9. Start PWM-kanal 1.
    PWM->PWM_ENA = SERVO_PWM_MASK;
}

void pwm_set_pulse_us(uint32_t pulse_us)
{
    // Hold pulsen innenfor servoens gyldige område.
    if (pulse_us < 900u) {
        pulse_us = 900u;
    }

    if (pulse_us > 2100u) {
        pulse_us = 2100u;
    }

    // Ny pulsbredde tas i bruk ved neste periodegrense.
    PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD =
        pulse_us;
}