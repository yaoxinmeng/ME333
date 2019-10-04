// Use the SPI peripheral to with encoder counting chip
// Provide functions for:
// 1. One-time setup of encoder module
// 2. Read the encoder in encoder counts
// 3. Read the encoder in degrees
// 4. Reset the encoder position so that the present angle is treated as 0

#ifndef ENCODER__H__
#define ENCODER__H__

#include <xc.h>

static int encoder_command(int read);
int encoder_counts(void);
void encoder_init(void);

#endif // ENCODER__H__
