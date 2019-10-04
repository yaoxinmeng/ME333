#include "utilities.h"
#define BUF_SIZE 200

static int mode;
struct current current_data;
struct angle position_data;

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
  current_data.reference[n] = reference;
  current_data.measured[n] = measured;

  // when sufficient samples are collected
  if ((n+1) == N){
    int i = 0;
    char message [BUF_SIZE];
    for (i = 0; i < N; i++){  // send plot data to MATLAB
      sprintf(message, "%d %d \r\n", current_data.reference[i], current_data.measured[i]);
      NU32_WriteUART3(message);
    }
  }
}

void set_trajectory(void){
  int i;
  char message [BUF_SIZE];

  NU32_ReadUART3(message, BUF_SIZE);               // read from client
  sscanf(message, "%d", &(position_data.size));    // save number of points in trajectory
  for (i = 0; i < position_data.size; i++)
  {
    NU32_ReadUART3(message, BUF_SIZE);              // read from client
    sscanf(message, "%f", &(position_data.reference[i]));
  }
}

float get_trajectory(int i){
  return position_data.reference[i];
}

void save_angledata(float measured, int n){
  int i = 0;
  char message [BUF_SIZE];

  position_data.measured[n] = measured;
  // when sufficient samples are collected
  if (!is_Track(n+1)){
    for (i = 0; i < position_data.size; i++){  // send plot data to MATLAB
      sprintf(message, "%f %f \r\n", position_data.reference[i], position_data.measured[i]);
      NU32_WriteUART3(message);
    }
  }
}

int is_Track(int n){    // checks if TRACK is over
  if (n < position_data.size){
    return  1;
  }
  else {
    return 0;
  }
}

int get_size(void){
  return position_data.size;
}
