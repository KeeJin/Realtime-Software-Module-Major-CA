//headerfile for waveform.c

#ifndef __WAVEFORM_H
#define __WAVEFORM_H

//Wave parameters
int wave_type;  //1 sine, 2 square, 3 triangular, 0 zero voltage
float amplitude;    //ampltude (0v to 5v)
float period;  //period in milliseconds
float vert_offset;
int duty_cycle;      //duty cycle (only affects square wave)


void *waveform_thread(void *arg);

#endif /* __WAVEFORM_H */
