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


pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;
pthread_t DisplayTUI_ID;


#if PCI
int pot_res = 32768;
#endif

#if PCIe
int pot_res = 65536;
#endif


void read_potentiometer(uint16_t *readpotentiometer1, uint16_t *readpotentiometer2) //function to read potentiometers
{
    #if PCIe
    out16(ADC_Data,0);		// Initiate Read #0
    delay(1);
    while(in8(ADC_Stat2) >0x80);	   			
    *readpotentiometer1=in16(ADC_Data);
    
    out16(ADC_Data,0);		// Initiate Read #1
    delay(1);
    while(in8(ADC_Stat2) >0x80);	    			
    *readpotentiometer2=in16(ADC_Data);

    #endif

    #if PCI
 	//start ADC
 	out16(AD_DATA,0);
 	while(!(in16(MUXCHAN)&0x4000));
 	*readpotentiometer1 = in16(AD_DATA); //read potentiometer 1
    
 	out16(AD_DATA,0);
 	while(!(in16(MUXCHAN)&0x4000));
 	*readpotentiometer2 = in16(AD_DATA); //read potentiometer 2
    #endif
 }


void update_LED(unsigned int dio_switch) //function to on LED based on switch states
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
    uint16_t readpotentiometer1;
    uint16_t readpotentiometer2;  
    unsigned int dio_switch_local;
    float amplitude_local;
    float period_local;
    int prev_switch0;

    #if PCI    
    dio_switch_local= in8(DIO_PORTA);
    #endif

    #if PCIe    
    dio_switch_local= in8(DIO_Data);
    #endif

	prev_switch0=switch0_value(dio_switch_local);
    while( switch0_value(dio_switch_local)==prev_switch0 )
    {


        update_LED(dio_switch_local);   //visualize the four switch states to the four LEDs        
        read_potentiometer(&readpotentiometer1,&readpotentiometer2);        //Read from potentiometer 1 and 2

        //Convert potentiometer readings to amplitude and average and update the variables
        //maximum potentiometer reading= pot_res (32768 or 65536)
        if(switch2_value(dio_switch_local))
        {
            amplitude_local = readpotentiometer1*1.0/pot_res * 5;           //map 0 to pot_res --> 0 to 5 volt   
            if(amplitude_local>5)amplitude_local=5; //capped at 5v to prevent overflow
            period_local = readpotentiometer2*1.0/pot_res * 25+25;    //map 0 to pot_res --> 25 to 50 
            current_amplitude = amplitude_local;
            current_period = period_local;
        }
        else
        {
            amplitude_local = prev_amplitude;
            period_local = prev_period;
        }
        
        #if PCI
		dio_switch_local = in8(DIO_PORTA);
		#endif
		
        #if PCIe
		dio_switch_local = in8(DIO_Data);
		#endif
		
		pthread_mutex_lock(&mutex_common);
        amplitude = amplitude_local;
        period = period_local;
        dio_switch = dio_switch_local;
        pthread_mutex_unlock(&mutex_common);
  
    }
}

