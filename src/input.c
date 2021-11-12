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
	 return (switch_value>>1)%2;
}

int switch2_value(int switch_value) //funciton to read 3rd switch --> toggle btw live & loaded
{
	 return (switch_value>>2)%2;
}

int switch3_value(int switch_value) //funciton to read 4th switch --> unused
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

    prev_switch0= switch0_value(dio_switch);
    while(1)
    {
        #if PCI
		dio_switch = in8(DIO_PORTA);
		#endif
		
        #if PCIe
		dio_switch = in8(DIO_Data); 
		#endif

        update_LED();   //visualize the four switch states to the four LEDs        

        if(switch1_value(dio_switch)) beeper = 1;
        else beeper = 0;


        if(switch2_value(dio_switch))
        {
            read_potentiometer();        //Read from potentiometer 1 and 2
            //Convert potentiometer readings to amplitude and average and update the variables
            //maximum potentiometer reading= pot_res (32768 or 65536)
            amplitude = readpotentiometer1*1.0/pot_res * 5;           //map 0 to pot_res --> 0 to 5 volt   
            if(amplitude>5)amplitude=5; //capped at 5v to prevent overflow
            period = readpotentiometer2*1.0/pot_res * 25+25;    //map 0 to pot_res --> 25 to 50 
        }
        else
        {
            amplitude = prev_amplitude;
            period = prev_period;
            duty_cycle = prev_duty_cycle;
        }
        

		if (switch0_value(dio_switch)!=prev_switch0)
        {
            fp = fopen("prev_wave.txt","w");
            fprintf(fp,"%d\n",wave_type);
            fprintf(fp,"%f\n",amplitude);
            fprintf(fp,"%f\n",period);
            fprintf(fp,"%f\n",vert_offset);
            fprintf(fp,"%d\n",duty_cycle);
            fclose(fp);
            
        	pthread_cancel(arrow_input_thread_ID);
        	wave_type = 4;
        	delay(period*100);
			pthread_cancel(waveform_thread_ID);
			pthread_cancel(app_tui_thread_ID);
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

