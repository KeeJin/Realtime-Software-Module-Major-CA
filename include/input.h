#ifndef INPUT_H
#define INPUT_H

#include "common.h"

FILE *fp;

/* ----------------------------- Functions -------------------------------- */
void *hardware_input_thread(void *arg); // Thread function for
                                        // interfacing with hardware peripherals

// Functions to convert the raw switch data into individual switch states
int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);

#endif /* INPUT_H */
