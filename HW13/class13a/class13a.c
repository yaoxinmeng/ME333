#include "NU32.h"          // constants, funcs for startup and UART

int main(void) {
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  LATF = 0x1; // turns off LED1 and turns on LED2

  while (1) {
    if (!PORTDbits.RD7) // USER is pressed
      LATF = 0x2; // turns on LED1 and turns off LED2
    else
      LATF = 0x1; // turns off LED1 and turns on LED2
  }
  return 0;
}
