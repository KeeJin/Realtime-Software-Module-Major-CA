// headerfile for input.c

#ifndef INPUT_H
#define INPUT_H

#include "common.h"

FILE *fp;

/* ----------------------------- Functions -------------------------------- */
void *hardware_input_thread(void *arg); // Thread function for
                                        // interfacing with hardware peripherals

// Function to read the potentiometer from the hardware
void read_potentiometer(uint16_t *readpotentiometer1,
                        uint16_t *readpotentiometer2);
>>>>>>> Stashed changes


// Function to write to LED on the hardware
void update_LED(unsigned int dio_switch);

// Functions to convert the raw switch data into individual switch states
int switch0_value(int switch_value);
int switch1_value(int switch_value);
int switch2_value(int switch_value);
int switch3_value(int switch_value);

#endif /* INPUT_H */
