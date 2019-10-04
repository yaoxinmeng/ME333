#include "currentcontrol.h"

static volatile struct current_gains gains;   // PI controller constants
static volatile float pwm = 0;          // user-defined pwm
static volatile int E_int = 0;         // Integral of error

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
  TRISDCLR = 0x2;          // set D1 as output

  // Default current gains
  gains.Kp = 830; gains.Ki = 230;
}

void set_pwm(float input){
  pwm = input;
}

void implement_pwm(void){
  float temp;
  temp = pwm / 100 * (PERIOD_REGISTER + 1);
  OC1RS = abs((int) temp);
  if (pwm < 0){
    LATDbits.LATD1 = 0x1;
  }
  else{
    LATDbits.LATD1 = 0x0;
  }
}

void set_current_gains(struct current_gains temp){
  gains = temp;
}

struct current_gains get_current_gains(void){
  return gains;
}

void current_PI(int reference, int actual){
  int e = reference - actual; // current error in mA
  int u = 0;                  // raw control signal (big value)
  float unew = 0;             // control signal in %

  E_int = E_int + e / 5;            // add error every 0.2ms, measured in microCoulumbs
  // anti-integrator windup
  if (gains.Ki * E_int > EINT_MAX){
    E_int = EINT_MAX / gains.Ki;
  }
  else if (gains.Ki * E_int < -EINT_MAX){
    E_int = -EINT_MAX / gains.Ki;
  }
  u = gains.Kp * e + gains.Ki * E_int;     // raw control signal

  // Convert control signal into a percentage
  unew = u / 2000.0;
  if (unew > 100.0){
    unew = 100.0;
  }
  else if (unew < -100){
    unew = -100.0;
  }
  set_pwm(unew);
  implement_pwm();
}
