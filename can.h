#include "cancontroller.h"
#include "spi.h"
#include <stdint.h>


#define CAN_MAX_DATA_SIZE 8

typedef struct {
    uint8_t id;
    uint8_t length;
    uint8_t data[CAN_MAX_DATA_SIZE];
} CanMessage;


void can_init(void);
void can_send(CanMessage *message);
CanMessage can_receive(void);

void can_test(void);
