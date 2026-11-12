#include "pwm.h"
#include "motor.h"
#include "sam.h"

/*
 * Registers used:
 * PMC_PCER0   : Enable the PIOB clock, bit 12.
 * PMC_PCER1   : Enable the PWM clock, bit 4 (peripheral ID 36).
 * PIO_ABSR    : Select peripheral function B, bit 13.
 * PIO_PDR     : Hand control of PB13 over to the peripheral, bit 13.
 * PWM_DIS     : Disable channel 1, bit 1.
 * PWM_SR      : Read channel status, bit 1.
 * PWM_CLK     : DIVA [7:0], PREA [11:8].
 * PWM_CMR     : CPRE [3:0], CALG bit 8, CPOL bit 9.
 * PWM_CPRD    : PWM period in clock ticks.
 * PWM_CDTY    : Pulse width in clock ticks.
 * PWM_CDTYUPD : Update the pulse width at the start of the next period.
 * PWM_ENA     : Enable channel 1, bit 1.
 */

#define SERVO_PWM_CHANNEL 1u
#define SERVO_PWM_MASK (1u << SERVO_PWM_CHANNEL)
#define SERVO_PIN_MASK (1u << 13)

/* ------------------ SERVO --------------------- */

void pwm_init(void) {
  // 1. Enable the peripheral clocks for PIOB and PWM.
  PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);
  PMC->PMC_PCER1 = (1u << (ID_PWM - 32u));

  // 2. Disable PWM channel 1 during configuration.
  PWM->PWM_DIS = SERVO_PWM_MASK;
  while (PWM->PWM_SR & SERVO_PWM_MASK) {
  }

  // 3. Select peripheral function B for PB13.
  PIOB->PIO_ABSR |= SERVO_PIN_MASK;

  // 4. Hand control of PB13 over to the peripheral.
  PIOB->PIO_PDR = SERVO_PIN_MASK;

  // 5. Set PWM clock A to 1 MHz by dividing 84 MHz by 84.
  PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84);

  // 6. Use clock A, left alignment, and a pulse that starts high.
  PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

  // 7. Set the period to 20,000 us (20 ms).
  PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CPRD = 20000u;

  // 8. A 1,500 us (1.5 ms) high pulse centers the servo.
  PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTY = 1500u;

  // 9. Start PWM channel 1.
  PWM->PWM_ENA = SERVO_PWM_MASK;
}

void pwm_set_pulse_us(uint32_t pulse_us) {
  // Keep the pulse width within the servo's valid range.
  if (pulse_us < 900u) {
    pulse_us = 900u;
  }

  if (pulse_us > 2100u) {
    pulse_us = 2100u;
  }

  // Apply the new pulse width at the start of the next PWM period.
  PWM->PWM_CH_NUM[SERVO_PWM_CHANNEL].PWM_CDTYUPD = pulse_us;
}

uint32_t joystickpos_to_pulse(uint8_t pos) {
  if (pos > 100u) {
    pos = 100u;
  }
  return 900u + (uint32_t)pos * 12u;
}

void pwm_pulse_from_controller(CanMsg *message) {
  uint8_t ypos = message->byte[1];

  // Y-axis controls the servo.
  pwm_set_pulse_us(joystickpos_to_pulse(ypos));
}