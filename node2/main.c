#include "can.h"
#include "diode.h"
#include "encoder.h"
#include "game.h"
#include "motor.h"
#include "pid.h"
#include "pwm.h"
#include "sam.h"
#include "solenoid.h"
#include "time.h"
#include "uart.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
  SystemInit();
  WDT->WDT_MR = WDT_MR_WDDIS;

  uart_init(84000000u, 9600u);

  motor_init();
  motor_set_x(50); // Stop, even before first CAN message.
  pwm_init();

  CanInit timing = {
      .phase2 = 3, .phase1 = 5, .propag = 4, .sjw = 0, .brp = 41, .smp = 0};
  can_init(timing, 0);

  adc_init(7);
  encoder_init();
  solenoid_init();

  run_game();
}
