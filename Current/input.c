// input.c --> changing wave parameter using hardware and arrowkey inputs

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <process.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>

#include "input.h"
#include "PCI_init.h"
#include "waveform.h"     

uint16_t readpotentiometer1;
uint16_t readpotentiometer2;
char ch =0;
float lower_limit;
float upper_limit;
float increment;

pthread_t arrow_input_thread_ID;
pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;

int beeper;

/*
#if PCI
int pot_res = 32768;
#endif

#if PCIe
int pot_res = 65536;
#endif
*/
int pot_res = 32768;


int switch_waveform(uint16_t switch_wave) //function to read switches
{
	#if PCI
    return (switch_wave>>2)-60; //shift 2, offset by 60 because the switches (1111switch1 switch2 **)
    #endif
	#if PCIe
    return (switch_wave>>2); //shift 2, offset by 60 because the switches (1111switch1 switch2 **)
    #endif
    // FLAGGED! TO CHANGE TO NCURSES INPUT
}

void read_potentiometer() //function to read potentiometers
{
    #if PCIe
    out16(ADC_Data,0);		// Initiate Read #0
    delay(1);
    while(in8(ADC_Stat2) >0x80);	   			
    readpotentiometer1=in16(ADC_Data);
    
    out16(ADC_Data,0);		// Initiate Read #1
    delay(1);
    while(in8(ADC_Stat2) >0x80);	    			
    readpotentiometer2=in16(ADC_Data);

    #endif

    #if PCI
 	//start ADC
 	out16(AD_DATA,0);
 	while(!(in16(MUXCHAN)&0x4000));
 	readpotentiometer1 = in16(AD_DATA); //read potentiometer 1
    
 	out16(AD_DATA,0);
 	while(!(in16(MUXCHAN)&0x4000));
 	readpotentiometer2 = in16(AD_DATA); //read potentiometer 2
    #endif
 }


void update_LED() //function to on LED based on switch states
{
    #if PCIe
    out8(DIO_Data, dio_switch);
    #endif

    #if PCI
    out8(DIO_PORTB, dio_switch);
    #endif  

}


int switch0_value(int switch_value) //funciton to read 1st switch --> kill code if different from starting state
{
	return switch_value%2;
}

int switch1_value(int switch_value) //funciton to read 2nd switch --> mute beeper
{
	 switch_value = (switch_value>>1)%2;
	 if(switch_value) beeper = 1;
	 else beeper = 0;
}

void *arrow_input_thread(void *arg) // thread to change vertical offset using up & down arrow keys 
{
		while(1)
        {
		    ch = getchar();
		    if(ch==65)	
            {
                vert_offset+=increment;
                if (vert_offset >= upper_limit) vert_offset = upper_limit;
                //printf("%.2f, Up", vert_offset);
            }
            if(ch==66)
            {
                vert_offset-=increment;
                if (vert_offset <= lower_limit) vert_offset = lower_limit;
                    //printf("%.2f, down", vert_offset);
            }
		}
		
}

void *hardware_input_thread(void *arg) // thread for digital I/O and potentiometer
{
    
    #if PCI    
    dio_switch= in8(DIO_PORTA);
    #endif

    #if PCIe    
    dio_switch= in8(DIO_Data);
    #endif

	switch0=switch0_value(dio_switch);
    switch0_prev=switch0;
    while(1)
    {
        update_LED();   //visualize the four switch states to the four LEDs        
        read_potentiometer();        //Read from potentiometer 1 and 2

        //Convert potentiometer readings to amplitude and average and update the variables
        //maximum potentiometer reading= pot_res (32768 or 65536)
        amplitude = readpotentiometer1*1.0/pot_res * 5;           //map 0 to pot_res --> 0 to 5 volt   
        if(amplitude>5)amplitude=5; //capped at 5v to prevent overflow
        period = readpotentiometer2*1.0/pot_res * 25+25;    //map 0 to pot_res --> 25 to 50 
        
        wave_type = switch_waveform(dio_switch); // FLAGGED! TO BE CHANGED TO NCURSES

        //print the wave parameters    
        system("clear");
 		printf("------------------------------------------------------------------\n");
	 	printf("Amplitude \t Period \t Vertical offset \n");
  		printf("------------------------------------------------------------------\n");
 	    printf("%.2f \t %.2f \t\t %.2f\n",amplitude,period,vert_offset); //SEND TO NCURSES
 	   
        
//        delay(1000);   
        //read SWITCH
        #if PCI
		dio_switch = in8(DIO_PORTA); // FLAGGED!
		#endif
		
        #if PCIe
		dio_switch = in8(DIO_Data); // FLAGGED!
		#endif
		
		
		switch0=switch0_value(dio_switch);
   		switch1_value(dio_switch);
		if (switch0!=switch0_prev)
        {
        	//kill ncurses input as well
        	pthread_cancel(arrow_input_thread_ID);
        	wave_type = 0;
        	delay(period*100);
			pthread_cancel(waveform_thread_ID);
			#if PCI
			out8(DIO_PORTB,0);
			#endif
			#if PCIe
			out8(DIO_Data,0);
			#endif

        	pci_detach_device(hdl);
   		 	exit(EXIT_SUCCESS);                                 //exit the program
            //is it possible to call a CTRL-C from here?
            //cause main.c handles ctrl-c signal
		}    
  
    }
}

