#ifndef COMMON
#define COMMON

#include <pthread.h>

#define LOWER_LIMIT_VOLTAGE -5.0f
#define UPPER_LIMIT_VOLTAGE 5.0f
#define VERT_OFFSET_INCREMENT 0.1f

typedef enum _waveType {
  SINE = 0,
  SQUARE = 1,
  TRIANGULAR = 2,
  SAWTOOTH = 3,
  ZERO = 4
} WaveType;

/* ----------------------------- Global params -------------------------------- */
WaveType wave_type;
float amplitude;
float period;  
float vertical_offset;
int time_period_ms;
extern pthread_mutex_t mutex_common;
extern pthread_mutex_t mutex_vertical_offset;
extern pthread_mutex_t mutex_wave_type;

WaveType current_wave_type;  
float current_amplitude;    
float current_period;  
float current_vert_offset;
int current_duty_cycle;     

WaveType prev_wave_type;  
float prev_amplitude;    
float prev_period;  
float prev_vert_offset;
int prev_duty_cycle;  

unsigned int dio_switch;
/* ---------------------------------------------------------------------------- */

#endif /* COMMON */
