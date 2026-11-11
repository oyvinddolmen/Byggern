#include "sam.h"
#include "can.h"
#include <stdio.h>

#define txMailbox 0
#define rxMailbox 1

void can_printmsg(CanMsg m)
{
    uint8_t length = m.length > 8 ? 8 : m.length;

    printf("CanMsg(id:%u, length:%u, data:{",
           (unsigned)m.id, (unsigned)m.length);

    for (uint8_t i = 0; i < length; i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("%u", (unsigned)m.byte[i]);
    }

    printf("})\n");
}

void can_init(CanInit init, uint8_t rxInterrupt)
{
    // Enable the PIOA and CAN0 peripheral clocks.
    PMC->PMC_PCER0 = (1u << ID_PIOA);
    PMC->PMC_PCER1 = (1u << (ID_CAN0 - 32));

    // Disable CAN while configuring it.
    CAN0->CAN_MR &= ~CAN_MR_CANEN;
    CAN0->CAN_IDR = 0xFFFFFFFFu;

    // Clear status by reading it.
    (void)CAN0->CAN_SR;

    // PA0 = CAN0 TX, PA1 = CAN0 RX, peripheral A.
    const uint32_t can_pins =
        PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;

    PIOA->PIO_IDR = can_pins;
    PIOA->PIO_ABSR &= ~can_pins;
    PIOA->PIO_PDR = can_pins;
    PIOA->PIO_PUER = can_pins;

    CAN0->CAN_BR = init.reg;

    // Transmit mailbox: standard 11-bit CAN frames.
    CAN0->CAN_MB[txMailbox].CAN_MMR = 0;
    CAN0->CAN_MB[txMailbox].CAN_MID = 0;
    CAN0->CAN_MB[txMailbox].CAN_MMR = CAN_MMR_MOT_MB_TX;

    // Receive mailbox: accept all identifiers.
    CAN0->CAN_MB[rxMailbox].CAN_MMR = 0;
    CAN0->CAN_MB[rxMailbox].CAN_MAM = 0;
    CAN0->CAN_MB[rxMailbox].CAN_MID = 0;
    CAN0->CAN_MB[rxMailbox].CAN_MMR = CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;

    if (rxInterrupt) {
        CAN0->CAN_IER = (1u << rxMailbox);
        NVIC_EnableIRQ(CAN0_IRQn);
    }

    CAN0->CAN_MR |= CAN_MR_CANEN;
}

void can_tx(CanMsg m)
{
    // Blocking: wait until the transmit mailbox is available.
    while (!(CAN0->CAN_MB[txMailbox].CAN_MSR & CAN_MSR_MRDY)) {
    }

    // Standard frame: do not set CAN_MID_MIDE.
    CAN0->CAN_MB[txMailbox].CAN_MID = CAN_MID_MIDvA(m.id);

    if (m.length > 8) {
        m.length = 8;
    }

    CAN0->CAN_MB[txMailbox].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[txMailbox].CAN_MDH = m.dword[1];

    CAN0->CAN_MB[txMailbox].CAN_MCR =
        ((uint32_t)m.length << CAN_MCR_MDLC_Pos) |
        CAN_MCR_MTCR;
}

uint8_t can_rx(CanMsg *m)
{
    if (m == NULL) {
        return 0;
    }

    uint32_t status = CAN0->CAN_MB[rxMailbox].CAN_MSR;

    if (!(status & CAN_MSR_MRDY)) {
        return 0;
    }

    uint32_t identifier = CAN0->CAN_MB[rxMailbox].CAN_MID;
    uint8_t length =
        (uint8_t)((status & CAN_MSR_MDLC_Msk) >>
                  CAN_MSR_MDLC_Pos);

    // Only accept standard data frames with up to eight bytes.
    if ((identifier & CAN_MID_MIDE) ||
        (status & CAN_MSR_MRTR) ||
        length > 8) {
        CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;
        return 0;
    }

    m->id =
        (uint8_t)((identifier & CAN_MID_MIDvA_Msk) >>
                  CAN_MID_MIDvA_Pos);
    m->length = length;

    m->dword[0] = CAN0->CAN_MB[rxMailbox].CAN_MDL;
    m->dword[1] = CAN0->CAN_MB[rxMailbox].CAN_MDH;

    // Release the mailbox for the next message.
    CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;

    return 1;
}