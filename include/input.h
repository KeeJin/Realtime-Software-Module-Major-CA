//headerfile for input.c

#ifndef __INPUT_H
#define __INPUT_H

#include <pthread.h>
typedef enum _graphType {
  SINE = 0,
  SQUARE = 1,
  TRIANGULAR = 2,
  SAWTOOTH = 3,
  ZERO = 4
} GraphType;

/* ----------------------------- Global params -------------------------------- */
GraphType graph_type;
float amplitude;
float frequency;
float phase_shift;
float vertical_offset;
int time_period_ms;
extern pthread_mutex_t mutex;
/* ---------------------------------------------------------------------------- */


void *hardware_input_thread(void *arg);

int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);

struct timespec start,stop;
float time_elapsed;
FILE *fp;

#endif /* __HARDWARE_INPUT_H */
