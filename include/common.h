#ifndef COMMON
#define COMMON

#include <pthread.h>

#define LOWER_LIMIT_VOLTAGE -5.0f
#define UPPER_LIMIT_VOLTAGE 5.0f
#define VERT_OFFSET_INCREMENT 0.1f

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
