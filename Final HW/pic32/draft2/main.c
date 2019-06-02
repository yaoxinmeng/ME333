#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"
 // include other header files here

#define BUF_SIZE 200

int main()
{
  char buffer[BUF_SIZE];

  // Initialisation functions
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  encoder_init(); // initialise SPI for encoder
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;

  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE);  // we expect the next character to be a menu command
    NU32_LED2 = 1;                    // clear the error LED
    switch (buffer[0]) {
      case 'c':                       // Read encoder (counts)
      {
        sprintf(buffer,"%d\r\n", encoder_counts());
        NU32_WriteUART3(buffer);      // send encoder count to client
        break;
      }
      case 'd':                       // Read encoder (deg)
      {
        sprintf(buffer,"%f\r\n", encoder_degs());
        NU32_WriteUART3(buffer);      // send encoder degrees to client
        break;
      }
      case 'e':                      // Reset encoder
      {
        encoder_reset();
        break;
      }
      case 'q':
      {
        // handle q for quit. Later you may want to return to IDLE mode here.
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
