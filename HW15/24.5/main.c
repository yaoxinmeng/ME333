#include "NU32.h"         // constants, functions for startup and UART

#define NUMSAMPS 1000     // number of points in waveform
#define PLOTPTS 200       // number of data points to plot
#define DECIMATION 10    // plots every 10th count

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS];  // measured values to plot
static volatile int REFarray[PLOTPTS];  // reference values to plot
static volatile int StoringData = 0;    // if this flag = 1, currently storing plot storing data
static volatile float Kp = 0, Ki = 0; // waveform

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) {
  static int counter = 0;     // counter is initialised once
  static int plotind = 0;     // index for data arrays - counts up to PLOTPTS
  static int decctr = 0;      // counts to store data once every DECIMATION
  static int adcval = 0;

  OC1RS = Waveform[counter];  // set duty cycle based on counter

  if (StoringData){
    decctr++;
    if (decctr == DECIMATION){
      decctr = 0;                             // reset decimation counter
      ADCarray[plotind] = adcval;             // store actual values
      REFarray[plotind] = Waveform[counter];  // store reference values
      plotind++;
    }
    if (plotind == PLOTPTS){
      plotind = 0;            // reset plot index
      StoringData = 0;        // tells main that data has finished storing
    }
  }

  counter++;
  if (counter == NUMSAMPS){
    counter = 0;
  }

  IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<8>
}

void makeWaveform();

int main(void) {
  // declare local variables
  char message[100];            // message to and from MATLAB
  float Kptemp = 0, Kitemp = 0; // temporary local gains
  int i = 0;                    // plot data loop counter

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
    NU32_ReadUART3(message, sizeof(message));   // wait for a message from MATLAB
    sscanf(message, "%f %f", &Kptemp, &Kitemp); // store MATLAB values to local gains

    __builtin_disable_interrupts(); // disable interrupts
    Kp = Kptemp; Ki = Kitemp;       // copy local to global values
    __builtin_enable_interrupts();  // enable interrupts

    StoringData = 1;                // message to ISR to start storing data
    while(StoringData){ ; }         // wait while ISR is storing data

    for (i = 0; i < PLOTPTS; i++){  // send plot data to MATLAB
      // when first integer sent is 1, MATLAB knows we're done
      sprintf(message, "%d %d %d \r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
      NU32_WriteUART3(message);
    }
  }
  return 0;
}

void makeWaveform(){
  int i = 0, center = (PR3 + 1)/2, A = center / 2;
  for (i = 0; i < NUMSAMPS; i++){
    if (i < NUMSAMPS/2){
        Waveform[i] = center + A;
    }
    else{
      Waveform[i] = center - A;
    }
  }
}
