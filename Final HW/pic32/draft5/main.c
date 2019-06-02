#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"       // encoder module
#include "utilities.h"     // utilities module
#include "isense.h"        // current sensor module
#include "currentcontrol.h"   // current controller module
#include "positioncontrol.h"  // position controller module

#define BUF_SIZE 200

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) CurrentController(void) {   // 5kHz inner loop
  // PID control goes here
  switch (get_mode()) {
    case 0:   // IDLE
    {
      // put H-bridge in brake mode with zero voltage across motor
      implement_pwm();
      break;
    }
    case 1:   // PWM
    {
      // implements fixed PWM cycle as requested by user
      implement_pwm();
      break;
    }
    case 2:   // ITEST
    {
      // tests current control loop only
      break;
    }
    case 3:   // HOLD
    {
      // hold motor at last position requested by user
      break;
    }
    case 4:   // TRACK
    {
      // track a reference motor trajectory specified by the user
      break;
    }
    default:  // Error
    {
      break;
    }
  }
  IFS0bits.T1IF = 0;            // clear interrupt flag IFS0<4>
}

void __ISR(_TIMER_2_VECTOR, IPL6SRS) PositionController(void) {   // 200Hz outer loop
  // PID control goes here
  IFS0bits.T2IF = 0;            // clear interrupt flag IFS0<8>
}

int main()
{
  char buffer[BUF_SIZE];

  // Initialisation functions
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  encoder_init(); // initialise SPI for encoder
  isense_init();  // initialise current sensor
  currentcontrol_init();  // initialise current control module
  set_mode(0);    // IDLE mode
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
      case 'a':
      {
        sprintf(buffer,"%d\r\n", current_counts());
        NU32_WriteUART3(buffer);      // send current count to client
        break;
      }
      case 'b':
      {
        sprintf(buffer,"%f\r\n", current_amps());
        NU32_WriteUART3(buffer);      // send current (mA) to client
        break;
      }
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
      case 'f':
      {
        float n = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);  // we expect PWM value from client
        sscanf(buffer, "%f", &n);
        set_mode(1);  // set mode to PWm
        set_pwm(n);   // set PWM
        break;
      }
      case 'p':
      {
        set_mode(0);    // put motor in IDLE mode
        break;
      }
      case 'q':
      {
        set_mode(0);    // put motor in IDLE mode
        break;
      }
      case 'r':
      {
        sprintf(buffer,"%d\r\n", get_mode()); // get current mode
        NU32_WriteUART3(buffer);              // send mode to client
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
