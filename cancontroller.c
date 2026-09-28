#include "cancontroller.h"


void can_read(uint8_t address, uint8_t *data, uint8_t length){
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_READ);
    spi_transfer_byte(address);

    for (uint8_t i = 0; i < length; i++) {
        data[i] = spi_transfer_byte(0xFF); // Send dummy byte to read data
    }

    spi_slave_select(NONE);
}


void can_write(uint8_t address, const uint8_t *data, uint8_t length){
    
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_WRITE);
    spi_transfer_byte(address);

    for (uint8_t i = 0; i < length; i++) {
        spi_transfer_byte(data[i]);
    }

    spi_slave_select(NONE);
}


void can_request_to_send(uint8_t tx_buffer_mask){
    
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_REQUEST_TO_SEND | tx_buffer_mask);
    spi_slave_select(NONE);
}


uint8_t can_read_status(void){
    
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_READ_STATUS);
    uint8_t status = spi_transfer_byte(0xFF); // Send dummy byte to read status
    spi_slave_select(NONE);
    return status;
}


void can_bit_modify(uint8_t address, uint8_t mask, uint8_t data){

    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_BIT_MODIFY);
    spi_transfer_byte(address);
    spi_transfer_byte(mask);
    spi_transfer_byte(data);
    spi_slave_select(NONE);
}


void can_reset(void){
    
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_RESET);
    spi_slave_select(NONE);
}