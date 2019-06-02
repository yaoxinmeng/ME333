#include "utilities.h"

void set_mode(int mode){
  switch (mode) {
    case IDLE:   // IDLE
    {
      // put H-bridge in brake mode with zero voltage across motor
      break;
    }
    case PWM:   // PWM
    {
      // implements fixed PWM cycle as requested by user
      break;
    }
    case ITEST:   // ITEST
    {
      // tests current control loop only
      break;
    }
    case HOLD:   // HOLD
    {
      // hold motor at last position requested by user
      break;
    }
    case TRACK:   // TRACK
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
