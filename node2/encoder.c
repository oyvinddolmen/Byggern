#include "sam.h"
#include "encoder.h"

void encoder_init(void)
{
    /*
     * Enable the peripheral clock for PIOC.
     * PC25 = encoder channel B
     * PC26 = encoder channel A
     */
    PMC->PMC_PCER0 = (1u << ID_PIOC);

    /*
     * Configure PC25 and PC26 for peripheral B.
     * Disable the PIO controller on these pins so that
     * the peripheral can use them.
     */
    const uint32_t encoder_pins = (1u << 25) | (1u << 26);

    PIOC->PIO_ABSR |= encoder_pins;
    PIOC->PIO_PDR = encoder_pins;

    /*
     * Enable the peripheral clock for TC channel 6.
     * TC channel 6 is channel 0 in the TC2 block.
     */
    PMC->PMC_PCER1 = (1u << (ID_TC6 - 32));

    /*
     * Enable quadrature decoding and position measurement.
     */
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN;

    /*
     * Select XC0 as the clock source for TC2 channel 0.
     */
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;

    /*
     * Enable and start the counter.
     */
    TC2->TC_CHANNEL[0].TC_CCR =
        TC_CCR_CLKEN | TC_CCR_SWTRG;
}

int32_t encoder_read(void)
{
    return (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}

int32_t encoder_read_position(int32_t left_limit, int32_t right_limit)
{
    int32_t range = left_limit - right_limit;
    if (range == 0 || range < 0) {
        return 0; // Avoid division by zero or invalid range
    }

    int32_t position = (100 * (float)encoder_read()) / (float)range;

    return position;
}