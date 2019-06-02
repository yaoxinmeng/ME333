// Use the ADC peripheral to sense motor current
// Provide functions for:
// 1. One-time setup of ADC
// 2. ADC value in ADC counts (0-1023)
// 3. ADC value in mA

#ifndef ISENSE__H__
#define ISENSE__H__

#include <xc.h>

#define SAMPLE_TIME 10  // 10 core timer ticks = 250 ns

void isense_init(void);
int current_counts(void);
float current_amps(void);

#endif // ISENSE__H__
