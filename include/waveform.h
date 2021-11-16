//headerfile for waveform.c

#ifndef WAVEFORM
#define WAVEFORM

//Wave parameters
float amplitude;    //ampltude (0v to 5v)
int duty_cycle;      //duty cycle (only affects square wave) --> fixed at 50% for now

int current_wave_type;  
float current_amplitude;    
float current_period;  
float current_vert_offset;
int current_duty_cycle;     

int prev_wave_type;  
float prev_amplitude;    
float prev_period;  
float prev_vert_offset;
int prev_duty_cycle;     

int beeper;

void *waveform_thread(void *arg);

#endif /* WAVEFORM */
