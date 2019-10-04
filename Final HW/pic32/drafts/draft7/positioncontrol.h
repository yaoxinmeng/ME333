// Use a timer to implement the 200Hz position control loop
// Provide functions for:
// 1. Initialise the module
// 2. Load trajectory from client
// 3. Load position control gains from client
// 4. Send position control gains back to client

#ifndef POSITIONCONTROL__H__
#define POSITIONCONTROL__H__

#include <xc.h>
#define DECIMATION 10
#define FREQUENCY 200
#define EINT_MAX 1000000         // maximum control effort is 50 mA

struct position_gains
{
   int Kp;
   int Ki;
   int Kd;
};

void positioncontrol_init(void);
void set_position_gains(struct position_gains temp);
struct position_gains get_position_gains(void);
void set_ref(float reference);
float get_ref(void);
void position_PID(float actual);
int get_desired_current(void);

#endif // POSITIONCONTROL__H__
