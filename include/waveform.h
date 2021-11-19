#ifndef WAVEFORM
#define WAVEFORM

#include "common.h"

/* ----------------------------- Functions -------------------------------- */
void *waveform_thread(void *arg);  // Thread function for generating waveforms
                                   // for output to oscilloscope

//Functions to generate waves
void sine_wave(unsigned int dio_switch_local, WaveType wave_type_local,
               float amplitude_local, float period_local,
               float vert_offset_local, int prev_switch0);

void square_wave(unsigned int dio_switch_local, WaveType wave_type_local,
                 float amplitude_local, float period_local,
                 float vert_offset_local, int duty_cycle_local,
                 int prev_switch0);
                
void triangular_wave(unsigned int dio_switch_local, WaveType wave_type_local,
                     float amplitude_local, float period_local,
                     float vert_offset_local, int prev_switch0);

void sawtooth_wave(unsigned int dio_switch_local, WaveType wave_type_local,
                   float amplitude_local, float period_local,
                   float vert_offset_local, int prev_switch0);
                   
//zero signal is used to "reset" the DAC to output nothing
void zero_signal(unsigned int dio_switch_local, WaveType wave_type_local,
                 float amplitude_local, float period_local,
                 float vert_offset_local, int prev_switch0);
/* ------------------------------------------------------------------------ */

#endif /* WAVEFORM */
