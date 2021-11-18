#ifndef WAVEFORM
#define WAVEFORM

#include "common.h"

int duty_cycle;      //duty cycle (only affects square wave) --> fixed at 50% for now   

void *waveform_thread(void *arg);

#endif /* WAVEFORM */
