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
#define BUF_SIZE 200
#define N 100     // number of sample points for TRACK or ITEST

struct current    // in mA
{
   int ref_current[N];
   int act_current[N];
};

struct position
{
   int ref_position[N];
   int act_position[N];
};

void set_mode(int input);
int get_mode(void);
void save_currentdata(int reference, int measured, int n);

#endif // UTILITIES__H__
