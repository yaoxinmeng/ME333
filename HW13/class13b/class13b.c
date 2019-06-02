#include "NU32.h"          // constants, funcs for startup and UART
#define MAX_MESSAGE_LENGTH 100
#define DELAY 20000000

int count = 1; // count number of presses

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  // write message to screen
  char message[MAX_MESSAGE_LENGTH];
  sprintf(message, "Pressed %d times\r\n", count++);
  NU32_WriteUART3(message);

  LATF = 0x0; // LED1 and LED2 on
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT() < DELAY) { ; } // delay for 20 M core ticks, 0.5 s
  LATF = 0x3; // LED1 and LED2 off

  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init
  LATF = 0x3; // LED1 and LED2 off

  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
  // Connect RD7 (USER button) to INT0 (RD0)
                                  
  while (1) { ; } // loops indefinitely

  return 0;
}
