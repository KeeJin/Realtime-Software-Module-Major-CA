//headerfile for input.c

#ifndef __INPUT_H
#define __INPUT_H

void *arrow_input_thread(void *arg); 
void *hardware_input_thread(void *arg);

struct timespec start,stop;
float time_elapsed;
FILE *fp;

#endif /* __HARDWARE_INPUT_H */
