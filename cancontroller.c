#include "cancontroller.h"
#include "spi.h"
#include <stddef.h>
#include <util/delay.h>


// les register i MCP2515 og lagre det i peker *data
void can_read(uint8_t address, uint8_t *data, uint8_t length){
    if (data == NULL || length == 0) return;
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_READ);
    spi_transfer_byte(address);

    for (uint8_t i = 0; i < length; i++) {
        data[i] = spi_transfer_byte(0xFF); // Send dummy byte to read data
    }

    spi_slave_select(NONE);
}

// Skrive for å konfigurere MCP2515
void can_write(uint8_t address, const uint8_t *data, uint8_t length){
    if (data == NULL || length == 0) return;
    
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_WRITE);
    spi_transfer_byte(address);

    for (uint8_t i = 0; i < length; i++) {
        spi_transfer_byte(data[i]);
    }

    spi_slave_select(NONE);
}

// ber MCP2515 om å sende melding som ligger i tx_buffer_mask
void can_request_to_send(uint8_t tx_buffer_mask){
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_REQUEST_TO_SEND | (tx_buffer_mask & 0x07));
    spi_slave_select(NONE);
}

/*
returnerer en byte hvor de ulike bitsene forteller status for TX og RX'ene
bit:     7    6    5    4    3    2    1    0
        -----------------------------------------
         -    -    -   TX2  TX1  TX0  RX1  RX0
høy bit betyr melding for RX0 og RX1, og sendeforespørsel for TX1 og RX2.
*/
uint8_t can_read_status(void){
    spi_slave_select(CAN);
    spi_transfer_byte(CAN_CMD_READ_STATUS);
    uint8_t status = spi_transfer_byte(0xFF); // Send dummy byte to read status
    spi_slave_select(NONE);
    return status;
}

// kan endre spesifikke bits i et register
// address = registeret, mask = hvilke bits
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
    _delay_ms(1); // Allow the controller to settle before register access.
}