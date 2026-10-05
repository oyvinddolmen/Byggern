#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include <stdint.h>
#include "spi.h"

typedef enum {
	CAN_CMD_READ = 0x03,
	CAN_CMD_WRITE = 0x02,
	CAN_CMD_REQUEST_TO_SEND = 0x80,
	CAN_CMD_READ_STATUS = 0xA0,
	CAN_CMD_BIT_MODIFY = 0x05,
	CAN_CMD_RESET = 0xC0
} CanCommand;

void can_read(uint8_t address, uint8_t *data, uint8_t length);
void can_write(uint8_t address, const uint8_t *data, uint8_t length);
void can_request_to_send(uint8_t tx_buffer_mask);
uint8_t can_read_status(void);
void can_bit_modify(uint8_t address, uint8_t mask, uint8_t data);
void can_reset(void);

#endif
