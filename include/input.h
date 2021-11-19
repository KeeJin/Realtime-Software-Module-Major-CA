// headerfile for input.c

#ifndef INPUT_H
#define INPUT_H

#include "common.h"

/* ----------------------------- Functions -------------------------------- */
void *hardware_input_thread(void *arg); // Thread function for
                                        // interfacing with hardware peripherals
                                        
// Functions for checking the status of individual digital switches
int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);
/* ------------------------------------------------------------------------ */

FILE *fp;

#endif /* INPUT_H */
