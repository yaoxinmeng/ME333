#include "NU32.h"          // constants, funcs for startup and UART
#define CORE_TICKS 4000000 // 4M ticks (10Hz)
#define MAX_MESSAGE_LENGTH 100 // max message length
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns
#define VOLTS_PER_COUNT (3.3/1024)

void __ISR(_CORE_TIMER_VECTOR, IPL6SRS) CoreTimerISR(void) { // step 1: the ISR
  IFS0bits.CTIF = 0;                // clear CT int flag IFS0<0>, same as IFS0CLR=0x0001
  unsigned int elapsed = 0, finish_time = 0;

  // ADC sampling
  AD1CHSbits.CH0SA = 0; // set MUXA to sample B0
  AD1CON1bits.SAMP = 1; // start sampling
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;
  while (_CP0_GET_COUNT() < finish_time) { ; }   // sample for more than 250 ns
  AD1CON1bits.SAMP = 0; // stop sampling and start converting
  while (!AD1CON1bits.DONE) { ; }     // wait for the conversion process to finish

  // Prints voltage
  char message[MAX_MESSAGE_LENGTH];
  float voltage = 0;
  voltage = ADC1BUF0 * VOLTS_PER_COUNT;
  sprintf(message, "B0 is %fV\r\n", voltage);
  NU32_WriteUART3(message);

  _CP0_SET_COUNT(0);                // set core timer counter to 0
  _CP0_SET_COMPARE(CORE_TICKS);     // must set CP0_COMPARE again after interrupt
}

int main(void) {
  NU32_Startup();         // cache on, min flash wait, interrupts on, LED/button init, UART init
  AD1PCFGbits.PCFG0 = 0;  // set B0 as analog input
  AD1CON3bits.ADCS = 2;   // ADC clock period is Tad = 2*(ADCS+1)*Tpb
                          //                         = 2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1;   // turn on A/D converter

  __builtin_disable_interrupts();   // step 2: disable interrupts at CPU
  _CP0_SET_COMPARE(CORE_TICKS);     // step 3: CP0_COMPARE register set to 40 M
  IPC0bits.CTIP = 6;                // step 4: interrupt priority
  IPC0bits.CTIS = 0;                // step 4: subp is 0, which is the default
  IFS0bits.CTIF = 0;                // step 5: clear CT interrupt flag
  IEC0bits.CTIE = 1;                // step 6: enable core timer interrupt
  __builtin_enable_interrupts();    // step 7: CPU interrupts enabled

  _CP0_SET_COUNT(0);                // set core timer counter to 0

  while(1) { ; }
  return 0;
}
