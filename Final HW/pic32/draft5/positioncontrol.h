// Use a timer to implement the 200Hz position control loop
// Provide functions for:
// 1. Initialise the module
// 2. Load trajectory from client
// 3. Load position control gains from client
// 4. Send position control gains back to client

#ifndef POSITIONCONTROL__H__
#define POSITIONCONTROL__H__

#include <xc.h>

void positioncontrol_init(void);

#endif // POSITIONCONTROL__H__
