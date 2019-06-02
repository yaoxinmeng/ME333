// This module implements the 5kHz current control loop
// Uses a timer for fixed-frequency control loop,
// a output compare + another timer to implement PWM,
// and a digital output to control motor direction
// Provide functions for:
// 1. One-time initialisation of all peripherals
// 2. Receive a fixed PWM command in the range of [-100,100]
// 3. Provide current control gains based on desired current and gains

#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

#include <xc.h>

#define PERIOD_REGISTER 3999  // for Timer3

void currentcontrol_init(void);
void set_pwm(float pwm);
void set_gains(float Kp_temp, float Ki_temp);
void currentcontrol_set(float current);

#endif // CURRENTCONTROL__H__
