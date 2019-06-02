// Use the SPI peripheral to with encoder counting chip
// Provide functions for:
// 1. One-time setup of encoder module
// 2. Read the encoder in encoder counts
// 3. Read the encoder in degrees
// 4. Reset the encoder position so that the present angle is treated as 0

#ifndef ENCODER__H__
#define ENCODER__H__

#include <xc.h>
#define DEGREES_PER_COUNT 360.0/4000
#define RESET_COUNT 32768

int encoder_counts(void);
float encoder_degs(void);
void encoder_init(void);
void encoder_reset(void);

#endif // ENCODER__H__
