#include "utilities.h"

static int mode;
struct current current_data;

void set_mode(int input){  // saves mode
  mode = input;
  switch (mode){
    case 0: // IDLE
    {
      set_pwm(0);
      break;
    }
    case 1: // PWM
    {
      break;
    }
    case 2: // ITEST
    {
      char message[BUF_SIZE];
      sprintf(message,"%d\r\n", N);
      NU32_WriteUART3(message);      // send sample number to client
      break;
    }
    case 3: // HOLD
    {
      break;
    }
    case 4: // TRACK
    {
      break;
    }
    default:
    {
      break;
    }
  }
}

int get_mode(void){       // returns mode
  return mode;
}

void save_currentdata(int reference, int measured, int n){
  current_data.ref_current[n] = reference;
  current_data.act_current[n] = measured;

  // when sufficient samples are collected
  if ((n+1) == N){
    int i = 0;
    char message [BUF_SIZE];
    for (i = 0; i < N; i++){  // send plot data to MATLAB
      sprintf(message, "%d %d \r\n", current_data.ref_current[i], current_data.act_current[i]);
      NU32_WriteUART3(message);
    }
  }
}
