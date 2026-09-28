#include "can.h"

void can_init(void){
    spi_init();
    can_reset();
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_WRITE);
    spi_transfer_byte(0x0F)         // address
    spi_transfer_byte(0x46);       // Loopback, retry, CLKOUT enabled, clock /4
    spi_slave_select(NONE);
}



void can_send(CanMessage *message){
    
}


CanMessage can_receive(void){

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