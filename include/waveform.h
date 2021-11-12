//headerfile for waveform.c

#ifndef __WAVEFORM_H
#define __WAVEFORM_H

//Wave parameters
int wave_type;  //0 sine, 1 square, 2 triangular, 3 sawtooth
float amplitude;    //ampltude (0v to 5v)
float period;  //period in milliseconds
float vert_offset;
int duty_cycle;      //duty cycle (only affects square wave)

//Saved wave parameters
int prev_wave_type;  
float prev_amplitude;    
float prev_period;  
float prev_vert_offset;
int prev_duty_cycle;      

unsigned int i;
unsigned int data;
int beeper;

int N=50; //number of "cuts" to make wave
int beep = 1; //to allow for only 1 beep per peak (for sine)

void *waveform_thread(void *arg);

#endif /* __WAVEFORM_H */
