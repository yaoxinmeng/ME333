#include "isense.h"

void isense_init(void){
  // Setup ADC
  AD1PCFGbits.PCFG0 = 0;  // set B0 as analog input
  AD1CON3bits.ADCS = 2;   // ADC clock period is Tad = 2*(ADCS+1)*Tpb = 2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1;   // turn on A/D converter
}

int current_counts(void){
  // ADC sampling
  AD1CHSbits.CH0SA = 0;                          // set MUXA to sample B0
  AD1CON1bits.SAMP = 1;                          // start sampling
  _CP0_SET_COUNT(0);                             // reset core timer
  while (_CP0_GET_COUNT() < SAMPLE_TIME) { ; }   // sample for more than 250 ns
  AD1CON1bits.SAMP = 0;                          // stop sampling and start converting
  while (!AD1CON1bits.DONE) { ; }                // wait for the conversion process to finish
  return ADC1BUF0;                               // stores adc value
}

float current_amps(void){
  return 0.0; // placeholder, require matlab best-fit plot
}
