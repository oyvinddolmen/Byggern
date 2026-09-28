#include "can.h"
#include "cancontroller.h"
#include "spi.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define MCP_CANSTAT   0x0E
#define MCP_CANCTRL   0x0F
#define MCP_CNF3      0x28
#define MCP_CANINTF   0x2C
#define MCP_RXB0CTRL  0x60
#define MCP_RXB1CTRL  0x70
#define MCP_TXB1CTRL  0x40
#define MCP_TXB1SIDH  0x41
#define MCP_RXB0SIDH  0x61
#define MCP_RXB1SIDH  0x71

#define MODE_MASK     0xE0
#define MODE_CONFIG   0x80
#define MODE_LOOPBACK 0x40
#define TX_REQUEST    0x08
#define RX0_FLAG      0x01
#define RX1_FLAG      0x02
#define RX_IDE        0x08
#define RX_SRR        0x10
#define TIMEOUT_MS    100

static bool wait_for_mode(uint8_t requested_mode)
{
    for (uint8_t elapsed = 0; elapsed < TIMEOUT_MS; elapsed++) {
        uint8_t status;
        can_read(MCP_CANSTAT, &status, 1);
        if ((status & MODE_MASK) == requested_mode) return true;
        _delay_ms(1);
    }
    return false;
}

bool can_init(void)
{
    spi_init();
    _delay_ms(1);
    can_reset(); // Enters Configuration mode; includes a delay

    // 125 kbit/s with a 16 MHz MCP2515 crystal
    const uint8_t timing[] = {0x03, 0xAC, 0x03};
    can_write(MCP_CNF3, timing, sizeof timing);

    // Accept all messages; allow RX0 to overflow into RX1
    const uint8_t receive_0 = 0x64;
    const uint8_t receive_1 = 0x60;
    can_write(MCP_RXB0CTRL, &receive_0, 1);
    can_write(MCP_RXB1CTRL, &receive_1, 1);

    const uint8_t control = 0x46; // Loopback, CLKOUT /4
    can_write(MCP_CANCTRL, &control, 1);

    return wait_for_mode(MODE_LOOPBACK);
}

bool can_send(const CanMessage *message)
{
    if (message == NULL || message->id > 0x7FF ||
        message->length > 8) {
        return false;
    }

    uint8_t status;
    can_read(MCP_TXB1CTRL, &status, 1);
    if (status & 0x08) return false; // Buffer busy

    uint8_t bytes[13] = {0};
    bytes[0] = message->id >> 3;
    bytes[1] = (message->id & 0x07) << 5;
    bytes[4] = message->length;
    memcpy(&bytes[5], message->data, message->length);

    can_write(MCP_TXB1SIDH, bytes, 5 + message->length);
    can_request_to_send(0x02);
    return true;
}

bool can_receive(CanMessage *message)
{
    if (message == NULL) return false;

    uint8_t flags;
    can_read(MCP_CANINTF, &flags, 1);
    if (!(flags & 0x01)) return false; // No message in RX0

    uint8_t bytes[13];
    can_read(MCP_RXB0SIDH, bytes, sizeof bytes);
    can_bit_modify(MCP_CANINTF, 0x01, 0); // Release RX0

    uint8_t length = bytes[4] & 0x0F;
    if ((bytes[1] & 0x18) || length > 8) {
        return false; // Discard extended or remote frames
    }

    message->id = ((uint16_t)bytes[0] << 3) | (bytes[1] >> 5);
    message->length = length;
    memcpy(message->data, &bytes[5], length);
    return true;
}

void can_test(void)
{
    if (!can_init()) {
        printf("CAN initialization failed\n");
        return;
    }
    const CanMessage sent = {.id = 0x321, .length = 2, .data = {0xAA, 0xBB}};
    if (!can_send(&sent)) {
        printf("CAN send failed\n");
        return;
    }
    CanMessage received;
    for (uint8_t elapsed = 0; elapsed < TIMEOUT_MS; elapsed++) {
        if (can_receive(&received)) {
            const bool matches = received.id == sent.id &&
                received.length == sent.length &&
                memcmp(received.data, sent.data, sent.length) == 0;
            printf(matches ? "CAN test passed\n" : "CAN test mismatch\n");
            return;
        }
        _delay_ms(1);
    }
    printf("CAN receive timeout\n");
}
