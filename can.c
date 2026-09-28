#include "can.h"
#include <stddef.h> // NULL
#include <stdio.h>  // printf

void can_init(void){
    spi_init();
    can_reset();
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_WRITE);
    spi_transfer_byte(0x0F)
    spi_transfer_byte(0x46);       // Loopback, retry, CLKOUT enabled, clock /4
    spi_transfer_byte(setup);
    spi_slave_select(NONE);
}

bool can_send(const CanMessage *message)
{
    if (message == NULL || message->id > 0x7FF ||
        message->length > 8) {
        return false;
    }

    // Do not overwrite a transmission already pending in buffer 1.
    uint8_t buffer_control;
    can_read(0x40, &buffer_control, 1);  // TXB1CTRL
    if (buffer_control & (1 << 3)) {     // TXREQ
        return false;
    }

    const uint8_t identifier_high = message->id >> 3;
    const uint8_t identifier_low = (message->id & 0x07) << 5;

    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_WRITE);
    spi_transfer_byte(0x41);             // TXB1SIDH

    spi_transfer_byte(identifier_high);  // 0x41: TXB1SIDH
    spi_transfer_byte(identifier_low);   // 0x42: TXB1SIDL; EXIDE = 0
    spi_transfer_byte(0x00);             // 0x43: TXB1EID8
    spi_transfer_byte(0x00);             // 0x44: TXB1EID0
    spi_transfer_byte(message->length);  // 0x45: TXB1DLC; RTR = 0

    for (uint8_t byte_index = 0; byte_index < message->length; byte_index++) {
        spi_transfer_byte(message->data[byte_index]); // 0x46 onward
    }
    spi_slave_select(NONE);

    can_request_to_send(0x02); // Request to send buffer 1

    return true;
}

bool can_receive(CanMessage *message)
{
    if (message == NULL) {
        return false;
    }

    uint8_t interrupt_flags;
    can_read(0x2C, &interrupt_flags, 1);  // CANINTF

    if ((interrupt_flags & (1 << 0)) == 0) { // RX0IF
        return false;
    }

    spi_slave_select(CAN);
    spi_transfer_byte(0x90); // READ RX BUFFER 0, starting at RXB0SIDH

    const uint8_t identifier_high = spi_transfer_byte(0xFF);
    const uint8_t identifier_low  = spi_transfer_byte(0xFF);
    spi_transfer_byte(0xFF);   // RXB0EID8
    spi_transfer_byte(0xFF);   // RXB0EID0
    const uint8_t data_length = spi_transfer_byte(0xFF); // RXB0DLC

    message->id = ((uint16_t)identifier_high << 3)
                | (identifier_low >> 5);
    message->length = data_length & 0x0F;

    if (message->length > 8) {
        spi_slave_select(NONE);
        return false;
    }

    for (uint8_t byte_index = 0; byte_index < message->length; byte_index++) {
        message->data[byte_index] = spi_transfer_byte(0xFF);
    }

    spi_slave_select(NONE); // Ends READ RX BUFFER and clears RX0IF
    return true;
}

void can_test(void){
    can_init();
    CanMessage message;
    message.id = 0x01;
    message.length = 2;
    message.data[0] = 0xAA;
    message.data[1] = 0xBB;

    can_send(&message);

    CanMessage received_message = can_receive();

    // Check if the received message matches the sent message
    if (received_message.id == message.id &&
        received_message.length == message.length &&
        received_message.data[0] == message.data[0] &&
        received_message.data[1] == message.data[1]) {
        // Test passed
        printf("CAN test passed!\n");
    } else {
        // Test failed
        printf("CAN test failed!\n");
    }
}