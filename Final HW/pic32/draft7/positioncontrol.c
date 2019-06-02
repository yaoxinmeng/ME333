#include "positioncontrol.h"

static volatile struct position_gains gains;  // PID controller constants
static volatile int E_int = 0, E_prev = 0, E_der = 0;     // Error variables
static volatile int current = 0;              // desired current in mA
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

  // Initialise default values for gains
  gains.Kp = 220;
  gains.Ki = 30;
  gains.Kd = 45;
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

void position_PID(float actual){
  int e = (int) ((ref_angle - actual) * FREQUENCY);   // 200x current error in degs
  int u = 0;                    // raw control signal (big value)
  static int counter = 0;

  E_int = E_int + e / FREQUENCY;            // add error every 5ms
  // anti-integrator windup
  if (gains.Ki * E_int > EINT_MAX){
    E_int = EINT_MAX / gains.Ki;
  }
  else if (gains.Ki * E_int < -EINT_MAX){
    E_int = -EINT_MAX / gains.Ki;
  }
  // implement E_der every 50ms to reduce spikes
  if (counter == DECIMATION){
    E_der = (e - E_prev) * (FREQUENCY/DECIMATION);       // find derivative of error every 50ms
    E_prev = e;     // save current error for next loop
    counter = 0;    // reset counter
  }
  counter ++;
  u = gains.Kp * e + gains.Ki * E_int + gains.Kd * E_der;     // raw control signal
  current = u / (100 * FREQUENCY);  // for gains of range 10-100, degs and mA are roughly on same order of magnitude
}

int get_desired_current(void){
  return current;
}
