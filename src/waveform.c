//waveform.c --> waveform functions are here
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>
															
#include "PCI_init.h" 
#include "waveform.h" 
#include "input.h"

unsigned int i;
unsigned int data;
int beeper;

int N=50; //number of "cuts" to make wave
int beeping = 1; //to allow for only 1 beep per peak (for sine)



void sine_wave() //sine wave function
{
    while( (wave_type==0) && (switch3_value(dio_switch)) )         //stops if wave_type is not 1 (sine)
    {
        for(i=0;i<N;i++) 
        {   
            if ( !(wave_type==0) || !(switch3_value(dio_switch)) ) return; 
            #if PCI
            data=( ( ( sinf((float)(i*2*3.1415/N))*amplitude)+10+vert_offset)/20.0  * 0xFFFF ); //send code here 
	        out16(DA_CTLREG,0x0923);			// DA Enable, #0, #1, SW 10V bipolar		2/6
   	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
            out16(DA_Data,(short) data); 
            #endif 

            #if PCIe
            data=( ( ( sinf((float)(i*2*3.1415/N))*amplitude)+10+vert_offset)/20.0  * 0x0FFF ); //send code here 
	        out16(DAC0_Data, data);
            #endif 

            delay((int)period);
            
            if ((sinf((float)(i*2*3.1415/N)) >= 0.99) && beeping) //beep if wave reaches max peak    
            {
                //printf("%.2f", sinf((float)(i*2*3.1415/N)));
                if(beeper) putchar(7);
                printf("\n");
                beeping = !beeping;
            }
           
           else if ((sinf((float)(i*2*3.1415/N)) <= -0.99) && !beeping)  //beep if wave reaches min peak 
           {
            //	printf("%.2f", sinf((float)(i*2*3.1415/N)));
                if(beeper) putchar(7);
                printf("\n");
                beeping = !beeping;
           }

																																																	
      	 }
        
    }
}


void square_wave() //square wave function
{
    while((wave_type==1)  && (switch3_value(dio_switch)))     //stops if wave_type is not 2 (square)
    {
        if(beeper) putchar(7); //beeps at first time wave reaches high for the cycle
        printf("\n"); 

        #if PCI
        data=( (amplitude+10+vert_offset) /20.0  * 0xFFFF );
        out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		2/6
        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
        out16(DA_Data,(short) data);
        #endif

        #if PCIe
        data=( (amplitude+10+vert_offset) /20.0  * 0x0FFF );
	    out16(DAC0_Data, data);
        #endif

        for(i=0;i<N*duty_cycle/100;i++)
        {
            if ( !(wave_type==1) || !(switch3_value(dio_switch)) ) return; 
            delay((int)period);
        }

        if(beeper) putchar(7);
        printf("\n"); //beeps at first time wave reaches low for the cycle
        
        #if PCI
        data=( (10-amplitude+vert_offset) /20.0  * 0xFFFF );
        out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		2/6
        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
        out16(DA_Data,(short) data);
        #endif
        
        #if PCIe
        data=( (10-amplitude+vert_offset) /20.0  * 0x0FFF );
	    out16(DAC0_Data, data);
        #endif

        for(i=0;i<N*(100-duty_cycle)/100;i++)
        {
            if ( !(wave_type==1) || !(switch3_value(dio_switch)) ) return; 
            delay((int)period);
        }


    }
}


void triangular_wave()
{
    while((wave_type==2) && (switch3_value(dio_switch)))     //stops if wave_type is not 3 (triangular)
    {
        for(i=0;i<N/2;i++) 
        {
            if ( !(wave_type==2) || !(switch3_value(dio_switch)) ) return; 
            #if PCI
            data=( (vert_offset -amplitude+ amplitude*4*i/N + 10)/20.0  * 0xFFFF );
	        out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		
   	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
            out16(DA_Data,(short) data);
            #endif

            #if PCIe
            data=( (vert_offset -amplitude+ amplitude*4*i/N + 10)/20.0  * 0x0FFF );
	        out16(DAC0_Data, data);
            #endif

			delay((int)period);		//micros																								
																																
  	    }
        if(beeper) putchar(7); //Beeps at peak
        printf("\n");

  	    for(i=0;i<N/2;i++) 
        {
            if ( !(wave_type==2) || !(switch3_value(dio_switch)) ) return; 
            #if PCI
            data=( (vert_offset + amplitude- amplitude*4*i/N + 10)/20.0  * 0xFFFF );
	        out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar
   	        out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
            out16(DA_Data,(short) data);
            #endif
            
            #if PCIe
            data=( (vert_offset + amplitude- amplitude*4*i/N + 10)/20.0  * 0x0FFF );
	        out16(DAC0_Data, data);
            #endif
            
			delay((int)period);		//micros																								
																																
  	    }
        if(beeper) putchar(7);//Beeps at min
        printf("\n");
    }
}
void sawtooth_wave()
{
    while((wave_type==3) && (switch3_value(dio_switch)))     //stops if wave_type is not 3 (triangular)
    {
        for(i=0;i<N;i++) 
        {
            if ( !(wave_type==3) || !(switch3_value(dio_switch)) ) return; 
            #if PCI
            data=( (vert_offset -amplitude+ amplitude*2*i/N + 10)/20.0  * 0xFFFF );
            out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		
            out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
            out16(DA_Data,(short) data);
            #endif

            #if PCIe
            data=( (vert_offset -amplitude+ amplitude*2*i/N + 10)/20.0  * 0x0FFF );
            out16(DAC0_Data, data);
            #endif

            delay((int)period);		//micros																								
                                                                                                                                
        }
        if(beeper) putchar(7); //Beeps at peak
        printf("\n");

    }    //sawtooth wave here
}

void zero_signal()
{
    #if PCI
    //data= (5 + vert_offset)/10* 0xFFFF;                       
    data = 1/2 * 0xFFFF;			//corresponds to 0 voltage signal
    out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		
    out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
    out16(DA_Data,(short) data);													
    #endif	
    
    #if PCIe
    //data= (5 + vert_offset)/10* 0x0FFF;                     
    data = 1/2 * 0x0FFF;			//corresponds to 0 voltage signal
    out16(DAC0_Data, data);
    #endif 		
    while( (wave_type == 4 ) || !(switch3_value(dio_switch)) )     //stops if wave_type is not 0 (zero voltage)
    {
  		#if PCI
    	//data= (5 + vert_offset)/10* 0xFFFF;                       
    	data = 1/2 * 0xFFFF;			//corresponds to 0 voltage signal
	    out16(DA_CTLREG,0x0923);			    // DA Enable, #0, #1, SW 10V bipolar		
   	    out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
        out16(DA_Data,(short) data);													
  		#endif	
        
  		#if PCIe
    	//data= (5 + vert_offset)/10* 0x0FFF;                     
    	data = 1/2 * 0x0FFF;			//corresponds to 0 voltage signal
        out16(DAC0_Data, data);
        #endif 														
    }
  
}


void *waveform_thread(void *arg)  //thread to generate wave based on wave parameters
{   
    while(1)
    {   if(switch3_value(dio_switch))
        {
            switch(wave_type)
            {
                case(SINE):    //sine
                    sine_wave();
                    break;
                case(SQUARE):    //square
                    square_wave();
                    break;
                case(TRIANGULAR):    //triangular
                    triangular_wave();
                    break;
                case(SAWTOOTH):    //sawtooth
                    sawtooth_wave();
                    break;
                case(ZERO):    //zero voltage
                    zero_signal();
                    break;
                default:
                    printf("invalid wave type error\n");
                    break;
            }
        }
        else zero_signal();
    } 
}
