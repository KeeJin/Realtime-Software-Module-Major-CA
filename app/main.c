// main.c --> command line arguments: wavetype and vertical offset
#ifndef HARDWARE
#include "terminal_ui.h"
#include <stdlib.h>

pthread_mutex_t mutex_common;

int main(void) {
  pthread_t display_thread;
  pthread_attr_t attr;
  int rc;
  long t;
  void* status;

  prev_wave_type = SINE;
  current_wave_type = SINE;
  prev_vert_offset = 0.0;
  current_vert_offset = 0.0;
  prev_duty_cycle = 50;
  current_duty_cycle = 50;
  dio_switch = 0;

  /* ------------------- Adjustable params ------------------- */
  time_period_ms = 100;
  amplitude = 3.0;
  period = 35.0;
  vertical_offset = 0.0;
  wave_type = SINE;
  duty_cycle = 50;
  /* ---------------------------------------------------------- */

  DisplayTUI();
  return 0;
}

#else

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <process.h>
#include <pthread.h>

#include "PCI_init.h"
#include "input.h"
#include "waveform.h"
#include "terminal_ui.h"

int j;
char colon = ':';
char argument;
char* argument_value;
pthread_mutex_t mutex_common;

pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;

// variable for file logging
// variables for finding the duration that the program runs

void signal_handler(int signum)  // Ctrl+c handler
{
  int rc;
  void* status;

  system("clear");
  if (current_period == 0) {
    current_amplitude = prev_amplitude;
    current_period = prev_period;
    current_duty_cycle = prev_duty_cycle;
  }
  fp = fopen("savefile.txt", "w");
  fprintf(fp, "%d\n%f\n%f\n%f\n%d\n", current_wave_type, current_amplitude,
          current_period, current_vert_offset, current_duty_cycle);
  fclose(fp);
  pci_detach_device(hdl);
  printf("Ending program...\n");
  printf("Resetting hardware...\n");
  pthread_mutex_lock(&mutex_common);
  wave_type = ZERO;
  pthread_mutex_unlock(&mutex_common);
  delay(period);
  pthread_cancel(waveform_thread_ID);
  pthread_cancel(hardware_input_thread_ID);

  rc = pthread_join(hardware_input_thread_ID, &status);
  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
  rc = pthread_join(waveform_thread_ID, &status);
  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
#if PCI
  out8(DIO_PORTB, 0);
#endif
#if PCIe
  out8(DIO_Data, 0);
#endif

  exit(EXIT_SUCCESS);  // exit the program
}

int main(int argc, char* argv[]) {
  pthread_attr_t attr;
  int rc;
  long t;
  void* status;
  int prev_switch0;

  // attach signal_handler to catch SIGINT
  signal(SIGINT, signal_handler);

  // Set Default Values of wave parameters
  wave_type = SINE;
  vertical_offset = 0.0;
  duty_cycle = 50;
  current_duty_cycle = 50;
  period = 50.0;

  fp = fopen("savefile.txt", "r");
  if (fp) {
    fscanf(fp, "%d %f %f %f %d", &prev_wave_type, &prev_amplitude, &prev_period,
           &prev_vert_offset, &prev_duty_cycle);
    if ((prev_wave_type != 1 && prev_wave_type != 2 && prev_wave_type != 3 &&
                   prev_wave_type != 0)
    || ( (prev_amplitude < 0) || (prev_amplitude > 5) ) 
    || ( (prev_period < 25) || (prev_period > 50) )    
    || ( (prev_vert_offset < -5) || (prev_vert_offset > 5) ) 
    || ( prev_duty_cycle < 0) || (prev_duty_cycle > 100) )
    {
	    prev_wave_type = 0;
	    prev_amplitude = 5.0;
	    prev_period = 50.0;
	    prev_vert_offset = 0.0;
	    prev_duty_cycle = 50;
    }

  }
  else {
    prev_wave_type = SINE;
    prev_amplitude = 5.0;
    prev_period = 50.0;
    prev_vert_offset = 0.0;
    prev_duty_cycle = 50;
  }
  // command line argument(s)
  for (j = 1; j < argc; j++) {
    // find colon
    if (argv[j][1] !=
        colon)  // invalid argument: colon not found at second character
    {
      printf("\n*******************************************************\n");
      printf("ERROR: Invalid command line argument\n");
      printf("Command line argument should be:\n");
      printf("./main t:wave_type v:vert_offset\n");
      printf("*******************************************************\n");
      return 0;  // invalid, exit program
    }

    // get the argument (wave_type or vertical_offset)
    argument = argv[j][0];

    // get the VALUE of the argument
    argument_value = &(argv[j][2]);
    switch (argument) {
      case ('v'):  // parse average/mean value and check whether it is of
                   // correct data type
        if (sscanf(argument_value, "%f", &vertical_offset) != 1) {
          printf("\n*******************************************************\n");
          printf("ERR: Vertical offset must be FLOAT,\n");
          printf("and must be between %.0f and %.0f\n", LOWER_LIMIT_VOLTAGE,
                 UPPER_LIMIT_VOLTAGE);
          printf("*******************************************************\n");
          return 0;  // invalid, exit program
        } else if (vertical_offset < LOWER_LIMIT_VOLTAGE ||
                   vertical_offset >
                       UPPER_LIMIT_VOLTAGE)  // check if average is in valid
                                             // range (-5 to 5)
        {
          printf("\n*******************************************************\n");
          printf("ERR: Invalid vertical offset!\n");
          printf("Vertical offset must be between %.0f and %.0f\n",
                 LOWER_LIMIT_VOLTAGE, UPPER_LIMIT_VOLTAGE);
          printf("*******************************************************\n");
          return 0;  // invalid, exit program
        }
        current_vert_offset = vertical_offset;
        break;

      case ('t'):
        if (sscanf(argument_value, "%d", &wave_type) !=
            1)  // parse wave type and check whether it is of correct data type
        {
          printf("\n*******************************************************\n");
          printf("ERR: Wave type must be INT (0,1,2,3)\n");
          printf("*******************************************************\n");
          return 0;  // invalid, exit program
        } else if (wave_type != SQUARE && wave_type != TRIANGULAR &&
                   wave_type != SAWTOOTH &&
                   wave_type != SINE)  // check if wave type value is valid
        {
          printf("\n*******************************************************\n");
          printf("ERR: Invalid wave type!\n");
          printf(
              "Input 0 for sine wave, 1 for square wave,\n"
              "2 for triangular wave, 3 for sawtooth wave\n");
          printf("*******************************************************\n");
          return 0;  // invalid, exit program
        }
        current_wave_type = wave_type;
        break;

      default:  // invalid
        printf("\n*******************************************************\n");
        printf("ERR: Invalid command line argument\n");
        printf("Command line argument should be as:\n");
        printf("./main t:wave_type v:vert_offset\n");
        printf("*******************************************************\n");
        return 0;  // invalid, exit program
        break;
    }
  }

  initialization();  // initialize hardware
  initialize_DIO();  // initialize Digital Input Output
  Initialize_ADC();  // initialize ADC

  /* ------------------- Adjustable params ------------------- */
  pthread_mutex_lock(&mutex_common);
  time_period_ms = 50;
  pthread_mutex_unlock(&mutex_common);
  /* ---------------------------------------------------------- */

#if PCI
  dio_switch = in8(DIO_PORTA);
#endif

#if PCIe
  dio_switch = in8(DIO_Data);
#endif

  prev_switch0 = switch0_value(dio_switch);
  /* ----- Initialize and set thread detached attribute ------- */

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  rc = pthread_create(&hardware_input_thread_ID, &attr, &hardware_input_thread,
                      (void*)t);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }

  rc = pthread_create(&waveform_thread_ID, &attr, &waveform_thread, (void*)t);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }

  pthread_attr_destroy(&attr);

  DisplayTUI();

  signal_handler(0);

  return 0;
}
#endif