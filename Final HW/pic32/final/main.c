#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"       // encoder module
#include "utilities.h"     // utilities module
#include "isense.h"        // current sensor module
#include "currentcontrol.h"   // current controller module
#include "positioncontrol.h"  // position controller module

#define BUF_SIZE 200

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) CurrentController(void) {   // 5kHz inner loop
  switch (get_mode()) {
    case 0:   // IDLE
    case 1:   // PWM
    {
      // implements fixed PWM cycle as requested by user
      implement_pwm();
      break;
    }
    case 2:   // ITEST
    { // tests current control loop only
      static int counter = 0, reference = 200;
      int measured = 0;  // in mA

      // create 100Hz square wave of 200mA amplitude (reference) for 20ms
      // 0.5 period of reference wave = 5ms = 25 ISR cycles
      if (counter % 25 == 24){
        reference = reference * -1;     // invert reference current every 5ms
      }
      measured = (int) current_amps();  // number of samples is N = 100, once every 0.2ms (everytime ISR is called)
      save_currentdata(reference, measured, counter);
      current_PI(reference, measured);  // set duty cycle based on PI Controller

      counter++;
      if (counter == 100){ // finished sampling
        set_mode(0);  // set mode to IDLE
        counter = 0;  // reset counter
      }
      break;
    }
    case 3:   // HOLD
    case 4:   // TRACK
    {
      // track a reference motor trajectory specified by the user
      int measured = (int) current_amps();  // in mA
      current_PI(get_desired_current(), measured);  // set duty cycle based on PI Controller
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
  if (get_mode() == 3){ // HOLD
    position_PID(encoder_degs());       // PID control based on measured angle
  }
  if (get_mode() == 4){ // TRACK
    static int counter = 0;
    float angle = encoder_degs();
    set_ref(get_trajectory(counter));   // set reference angle based on counter
    save_angledata(angle,counter);      // save data to utilities
    position_PID(angle);                // PID control based on measured angle

    counter++;
    if (!is_Track(counter)){   // finished sampling
      set_ref(get_trajectory(counter));     // set reference angle to last value in array
      set_mode(3);                          // set mode to HOLD
      counter = 0;                          // reset counter
    }
  }
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
  positioncontrol_init(); // initialise position control module
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
      case 'f':                      // Set PWM
      {
        float n = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);  // we expect PWM value from client
        sscanf(buffer, "%f", &n);
        set_mode(1);  // set mode to PWM
        set_pwm(n);   // set PWM
        break;
      }
      case 'g':                      // Set current gains
      {
        struct current_gains temp;        // struct to hold gains
        NU32_ReadUART3(buffer, BUF_SIZE);  // we expect current gains from client
        sscanf(buffer, "%d %d", &(temp.Kp), &(temp.Ki));   // save gains
        set_current_gains(temp);
        break;
      }
      case 'h':                      // Get current gains
      {
        struct current_gains temp;    // struct to hold gains
        temp = get_current_gains();
        sprintf(buffer,"%d %d\r\n", temp.Kp, temp.Ki);
        NU32_WriteUART3(buffer);      // send current gains to client
        break;
      }
      case 'i':                      // Set position gains
      {
        struct position_gains temp;        // struct to hold gains
        NU32_ReadUART3(buffer, BUF_SIZE);  // we expect position gains from client
        sscanf(buffer, "%d %d %d", &(temp.Kp), &(temp.Ki), &(temp.Kd));   // save gains
        set_position_gains(temp);
        break;
      }
      case 'j':                      // Get position gains
      {
        struct position_gains temp;    // struct to hold gains
        temp = get_position_gains();
        sprintf(buffer,"%d %d %d \r\n", temp.Kp, temp.Ki, temp.Kd);
        NU32_WriteUART3(buffer);      // send current gains to client
        break;
      }
      case 'k':                      // Test current gains
      {
        set_mode(2);  // set mode to ITEST
        break;
      }
      case 'l':                      // Go to angle (degs)
      {
        float angle = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);  // we expect angle from client
        sscanf(buffer, "%f", &angle);   // save angle
        set_ref(angle);
        set_mode(3);  // set mode to HOLD
        break;
      }
      case 'm':                      // Set step trajectory
      case 'n':                      // Set cubic trajectory
      {   // fall through from previous since they are the same
        set_trajectory();
        break;
      }
      case 'o':
      {
        sprintf(buffer,"%d\r\n", get_size());  // get sample size
        NU32_WriteUART3(buffer);                // send sample size to client
        set_mode(4);    // set to TRACK mode
        break;
      }
      case 'p':                      // Unpower motor
      case 'q':                      // Quit
      {   // fall through from previous since they are the same
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
