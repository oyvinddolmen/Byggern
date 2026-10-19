#include "sam.h"
#include "pwm.h"
#include "motor.h"

/*
 * Registre brukt:
 * PMC_PCER0   : Aktiver PIOB-klokken, bit 12.
 * PMC_PCER1   : Aktiver PWM-klokken, bit 4 (periferi-ID 36).
 * PIO_ABSR    : Velg periferifunksjon B, bit 13.
 * PIO_PDR     : Overlat PB13 til periferien, bit 13.
 * PWM_DIS     : Deaktiver kanal 1, bit 1.
 * PWM_SR      : Les kanalstatus, bit 1.
 * PWM_CLK     : DIVA [7:0], PREA [11:8].
 * PWM_CMR     : CPRE [3:0], CALG bit 8, CPOL bit 9.
 * PWM_CPRD    : PWM-perioden i klokketikk.
 * PWM_CDTY    : Pulsbredden i klokketikk.
 * PWM_CDTYUPD : Oppdater pulsbredde ved neste periode.
 * PWM_ENA     : Aktiver kanal 1, bit 1.
 */

#define SERVO_PWM_CHANNEL  1u
#define SERVO_PWM_MASK     (1u << SERVO_PWM_CHANNEL)
#define SERVO_PIN_MASK     (1u << 13)



/* ------------------ SERVO --------------------- */

void pwm_init(void)
{
    // 1. Aktiver periferiklokkene til PIOB og PWM.
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);
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

    // 8. Høy puls: 1 500 us = 1,5 ms gir servo til senter posisjon
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

uint32_t joystickpos_to_pulse(uint8_t pos)
{
    if (pos > 100u) {
        pos = 100u;
    }
    printf("%u\n", (uint32_t)pos);
    return 900u + (uint32_t)pos * 12u;
}


void pwm_pulse_from_controller(CanMsg *message)
{
    uint8_t xpos = message->byte[0];
    uint8_t ypos = message->byte[1];


    // Y-axis controls the servo.
    pwm_set_pulse_us(joystickpos_to_pulse(ypos));

    // X-axis controls the motor.
    motor_set_x(xpos);
}