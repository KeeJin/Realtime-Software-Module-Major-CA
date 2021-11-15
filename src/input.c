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

pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;
pthread_t DisplayTUI_ID;

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
int switch2_value(int switch_value) //funciton to view live/prev values
{
	return (switch_value>>2)%2;
}

int switch3_value(int switch_value) //function to view in scope/osci
{
	return (switch_value>>3)%2;
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
        if(switch2_value(dio_switch))
        {
            amplitude = readpotentiometer1*1.0/pot_res * 5;           //map 0 to pot_res --> 0 to 5 volt   
            if(amplitude>5)amplitude=5; //capped at 5v to prevent overflow
            period = readpotentiometer2*1.0/pot_res * 25+25;    //map 0 to pot_res --> 25 to 50 
            current_amplitude = amplitude;
            current_period = period;
        }
        else
        {
            amplitude = prev_amplitude;
            period = prev_period;
        }
        
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
            pthread_cancel(DisplayTUI_ID);
            endwin();
            system("clear");
            printf("Ending program...\n");
            printf("Resetting hardware...\n");
            fp = fopen("savefile.txt","w");
            fprintf(fp,"%d\n%f\n%f\n%f\n%d\n",current_wave_type,current_amplitude,current_period,current_vert_offset,duty_cycle);
            fclose(fp);
            pci_detach_device(hdl);
            wave_type = 4;
            delay(period*100);
            pthread_cancel(waveform_thread_ID);
            #if PCI
            out8(DIO_PORTB,0);
            #endif
            #if PCIe
            out8(DIO_Data,0);
            #endif
            
            //get the time when the program stops
            if(clock_gettime(CLOCK_REALTIME,&stop)==-1)
            { 
                printf("clock gettime stop error");
            }
            
            //compute duration that program has run
            time_elapsed=(double)(stop.tv_sec-start.tv_sec)+ (double)(stop.tv_nsec- start.tv_nsec)/1000000000;
            
            ///create/open log.txt for logging & log exit message and duration that the program has run
            fp = fopen("log.txt","a");
            fprintf(fp,"Ending program \n");
            fprintf(fp,"Program runs for %lf seconds \n\n",time_elapsed);
            fclose(fp);
            
            exit(EXIT_SUCCESS);                                 //exit the program
		}    
  
    }
}

