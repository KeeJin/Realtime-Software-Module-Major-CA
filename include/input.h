//headerfile for input.c

#ifndef __INPUT_H
#define __INPUT_H

void *arrow_input_thread(void *arg); 
void *hardware_input_thread(void *arg);

uint16_t readpotentiometer1;
uint16_t readpotentiometer2;
char ch =0;
float lower_limit = -5;
float upper_limit = 5;
float increment =0.1;
FILE *fp; //load & save

pthread_t arrow_input_thread_ID;
pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;
pthread_t app_tui_thread_ID;

int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);

#endif /* __HARDWARE_INPUT_H */
