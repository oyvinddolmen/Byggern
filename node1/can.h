#ifndef CAN_H
#define CAN_H

#include "cancontroller.h"
#include <stdbool.h>
#include <stdint.h>

#define CAN_MAX_DATA_SIZE 8

typedef struct {
  uint16_t id; // 11-bit standard identifier
  uint8_t length;
  uint8_t data[CAN_MAX_DATA_SIZE];
} CanMessage;

bool can_init(void);
bool can_send(const CanMessage *message);
bool can_receive(CanMessage *message);
void can_test(void);

#endif