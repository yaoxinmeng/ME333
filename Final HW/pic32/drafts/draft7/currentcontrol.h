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
#define EINT_MAX 100000       // maximum control effort 

struct current_gains
{
   int Kp;
   int Ki;
};

void currentcontrol_init(void);
void set_pwm(float input);
void implement_pwm(void);
void set_current_gains(struct current_gains temp);
struct current_gains get_current_gains(void);
void current_PI(int reference, int actual);

#endif // CURRENTCONTROL__H__
