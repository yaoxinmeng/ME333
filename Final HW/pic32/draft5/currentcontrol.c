#include "currentcontrol.h"

static volatile float Kp = 0, Ki = 0;   // PI controller constants
static volatile float pwm = 0;          // user-defined pwm

void currentcontrol_init(void){
  // Setup OC1 and Timer3
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1
  PR3 = PERIOD_REGISTER;   // period = 80Mhz /((PR3+1) * N) = 20kHz
  TMR3 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;   // use Timer3 for OC1
  OC1CONbits.OCM = 0b110;  // PWM mode with fault pin disabled
  OC1RS = 3000;            // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 3000;             // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  // Setup Timer1 for interrupt
  __builtin_disable_interrupts(); // disable interrupts
  T1CONbits.TCKPS = 0;            // Timer1 prescaler N=1
  PR1 = 15999;                    // period = 80Mhz /((PR1+1) * N) = 5kHz
  TMR1 = 0;                       // initial TMR1 count is 0
  T1CONbits.ON = 1;               // turn on Timer1
  IPC1bits.T1IP = 5;              // interrupt priority 5
  IPC1bits.T1IS = 1;              // interrupt subpriority 1
  IFS0bits.T1IF = 0;              // clear the int flag
  IEC0bits.T1IE = 1;              // enable T1 by setting IEC0<4>
  __builtin_enable_interrupts();  // enable interrupts

  // Setup digital output
  TRISDCLR = 0x1;          // set D1 as output
}

void set_pwm(float input){
  pwm = input;
}

void implement_pwm(void){
  float temp;
  temp = pwm / 100 * (PERIOD_REGISTER + 1);
  OC1RS = (int) temp;
  if (pwm < 0){
    LATDbits.LATD1 = 1;
  }
  else{
    LATDbits.LATD1 = 0;
  }
}

void set_gains(float Kp_temp, float Ki_temp){
  Kp = Kp_temp;
  Ki = Ki_temp;
}

void currentcontrol_set(float current){
    ;
}
