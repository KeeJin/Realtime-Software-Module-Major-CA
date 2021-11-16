#ifndef COMMON
#define COMMON

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

#endif /* COMMON */
