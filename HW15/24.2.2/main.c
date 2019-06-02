#include "NU32.h"         // constants, functions for startup and UART

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init

  // Setup OC1 and Timer3
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1
  PR3 = 3999;              // period = 80Mhz /((PR3+1) * N) = 20kHz
  TMR3 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;   // use Timer3 for OC1
  OC1CONbits.OCM = 0b110;  // PWM mode with fault pin disabled
  OC1RS = 3000;            // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 3000;             // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  while(1) {
    ;                      // infinite loop
  }
  return 0;
}
