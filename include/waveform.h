//headerfile for waveform.c

#ifndef __WAVEFORM_H
#define __WAVEFORM_H

//Wave parameters
int wave_type;  //0 sine, 1 square, 2 triangular, 3 sawtooth
float amplitude;    //ampltude (0v to 5v)
float period;  //period in milliseconds
float vert_offset;
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


void *waveform_thread(void *arg);

#endif /* __WAVEFORM_H */
