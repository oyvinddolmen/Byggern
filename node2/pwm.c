#include "sam.h"
#include "pwm.h"

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

// void pwm_pulse_from_controller(void){

//     CanMsg message;
    
//     bool recieved = can_rx(&message);
//     if (recieved){
//         printf("Byte 0: %u, Byte 1: %u\n",
//                (unsigned int)message.byte[0],
//                (unsigned int)message.byte[1]);

//         uint8_t ypos = message.byte[1];
//         pwm_set_pulse_us(joystickpos_to_pulse(ypos));
//     };
// }


/* ------------------ MOTOR --------------------- */
#include "sam.h"
#include <stdint.h>

#define MOTOR_PWM_MAX 100

void motor_init(void)
{

    // Enable clocks for PIOB, PIOC and PWM.
    PMC->PMC_PCER0 = (1u << ID_PIOB) |
                     (1u << ID_PIOC);

    PMC->PMC_PCER1 = (1u << (ID_PWM - 32u));

    // Configure PC23 as GPIO output for motor direction.
    PIOC->PIO_PER = (1u << 23);
    PIOC->PIO_OER = (1u << 23);
    PIOC->PIO_CODR = (1u << 23);

    // Assign PB12 to PWM peripheral B (motor ENABLE).
    PIOB->PIO_ABSR |= (1u << 12);
    PIOB->PIO_PDR = (1u << 12);

    // Configure PWM channel 0, preserving the shared PWM clock.
    PWM->PWM_DIS = PWM_DIS_CHID0;

    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM->PWM_CH_NUM[0].PWM_CPRD = 1000u;
    PWM->PWM_CH_NUM[0].PWM_CDTY = 0u;

    // Enable motor PWM channel.
    PWM->PWM_ENA = PWM_ENA_CHID0;
}

void motor_set_x(uint8_t x)
{
    if (x > 100)
        x = 100;

    uint32_t duty;

    if (x >= 20 && x <= 80)
    {
        duty = 0;
    }
    else if (x < 50)
    {
        PIOC->PIO_CODR = (1u << 23);  // PHASE low
        duty = (uint32_t)(50 - x) * 20u;
    }
    else
    {
        PIOC->PIO_SODR = (1u << 23);  // PHASE high
        duty = (uint32_t)(x - 50) * 20u;
    }

    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = duty;

    printf("X=%u, duty=%lu\n",
           (unsigned int)x,
           (unsigned long)duty);
}




void controller_update(void)
{
    CanMsg message;

    if (can_rx(&message))
    {
        uint8_t xpos = message.byte[0];
        uint8_t ypos = message.byte[1];

        // Y-axis controls the servo.
        pwm_set_pulse_us(joystickpos_to_pulse(ypos));

        // X-axis controls the motor.
        motor_set_x(xpos);
    }
}