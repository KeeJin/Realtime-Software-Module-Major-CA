//headerfile for input.c

#ifndef __INPUT_H
#define __INPUT_H

#include "common.h"

void *hardware_input_thread(void *arg);

int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);

struct timespec start,stop;
float time_elapsed;
FILE *fp;

#endif /* __HARDWARE_INPUT_H */
