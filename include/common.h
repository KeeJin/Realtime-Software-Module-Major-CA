#ifndef COMMON
#define COMMON

#include <pthread.h>

// Global constants defined as macros
#define LOWER_LIMIT_VOLTAGE -5.0f
#define UPPER_LIMIT_VOLTAGE 5.0f
#define VERT_OFFSET_INCREMENT 0.1f
#define DUTY_CYCLE_INCREMENT 5

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
int duty_cycle;      
extern pthread_mutex_t mutex_common;

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
