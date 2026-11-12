#ifndef SOLENOID_H
#define SOLENOID_H

#include "can.h"

void solenoid_init(void);

void solenoid_activate(CanMsg *message);

void solenoid_update(void);

#endif
