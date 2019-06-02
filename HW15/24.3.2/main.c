#include "NU32.h"         // constants, functions for startup and UART

#define NUMSAMPS 1000     // number of points in waveform

static volatile int Waveform[NUMSAMPS]; // waveform

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
  static int counter = 0;

  OC1RS = Waveform[counter];  // set duty cycle based on counter
  counter++;
  if (counter == NUMSAMPS){
    counter = 0;
    NU32_LED1 = !NU32_LED1;   // Use LED1 for debugging
  }

  IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<8>
}

void makeWaveform();

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

  // Setup Timer2 for interrupt
  __builtin_disable_interrupts(); // disable interrupts
  T2CONbits.TCKPS = 1;            // Timer3 prescaler N=2
  PR2 = 39999;                    // period = 80Mhz /((PR2+1) * N) = 1kHz
  TMR2 = 0;                       // initial TMR2 count is 0
  T2CONbits.ON = 1;               // turn on Timer2
  IPC2bits.T2IP = 5;              // interrupt priority 5
  IPC2bits.T2IS = 1;              // interrupt subpriority 1
  IFS0bits.T2IF = 0;              // clear the int flag
  IEC0bits.T2IE = 1;              // enable T2 by setting IEC0<8>
  __builtin_enable_interrupts();  // enable interrupts

  makeWaveform(); // initialise waveform

  while(1) {
    ;                      // infinite loop
  }
  return 0;
}

void makeWaveform(){
  int i = 0, center = (PR3 + 1)/2, A = center/2;
  for (i = 0; i < NUMSAMPS; i++){
    if (i < NUMSAMPS/2){
        Waveform[i] = center + A;
    }
    else{
      Waveform[i] = center - A;
    }
  }
}
