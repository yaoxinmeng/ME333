#include "positioncontrol.h"

static volatile struct position_gains gains;  // PID controller constants
static volatile int Eint = 0, E_prev = 0;     // Integral of error
static volatile float ref_angle;              // reference angle

void positioncontrol_init(void){
  // Setup Timer2 for interrupt
  __builtin_disable_interrupts(); // disable interrupts
  T2CONbits.TCKPS = 0b100;        // Timer2 prescaler N=16
  PR2 = 24999;                    // period = 80Mhz /((PR2+1) * N) = 200Hz
  TMR2 = 0;                       // initial TMR2 count is 0
  T2CONbits.ON = 1;               // turn on Timer2
  IPC2bits.T2IP = 6;              // interrupt priority 6
  IPC2bits.T2IS = 1;              // interrupt subpriority 1
  IFS0bits.T2IF = 0;              // clear the int flag
  IEC0bits.T2IE = 1;              // enable T2 by setting IEC0<8>
  __builtin_enable_interrupts();  // enable interrupts
}

void set_position_gains(struct position_gains temp){
  gains = temp;
}

struct position_gains get_position_gains(void){
  return gains;
}

void set_ref(float reference){
  ref_angle = reference;
}

float get_ref(void){
  return ref_angle;
}
