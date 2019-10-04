// Used for various bookkeeping tasks
// Has a variable that holds the operating mode (IDLE,PWM,ITEST,HOLD,TRACK)
// Has arrays that collect data from trajectory tracking (TRACK) or current tracking (ITEST)
// Provide functions for:
// 1. Set operating mode
// 2. Receive the number N of samples to save into data buffers during TRACK or ITEST
// 3. Write data to buffers if N is not yet reached
// 4. Send buffer data to client when N is reached

#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h>
#define MAX_TRAJECTORY_SIZE 2000
#define N 100     // number of sample points for TRACK or ITEST

struct current    // in mA
{
   int reference[N];
   int measured[N];
};

struct angle
{
   int size;
   float reference[MAX_TRAJECTORY_SIZE];
   float measured[MAX_TRAJECTORY_SIZE];
};

void set_mode(int input);
int get_mode(void);
void save_currentdata(int reference, int measured, int n);
void set_trajectory(void);
float get_trajectory(int i);
void save_angledata(float measured, int n);
int is_Track(int n);
int get_size(void);

#endif // UTILITIES__H__
