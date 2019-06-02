#include "utilities.h"

static int input;   // private variable

void set_mode(int mode){  // saves mode
  input = mode;

  char buffer[BUF_SIZE];
  switch (mode) {
    case 0:   // IDLE
    {
      // put H-bridge in brake mode with zero voltage across motor
      set_pwm(0);
      break;
    }
    case 1:   // PWM
    {
      // implements fixed PWM cycle as requested by user
      float n = 0;
      NU32_ReadUART3(buffer,BUF_SIZE);  // we expect a pwm value
      sscanf(buffer,'%f', &n);
      set_pwm(n);
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
}

int get_mode(void){       // returns mode
  return input;
}
