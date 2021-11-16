// main.c --> command line arguments: wavetype and vertical offset

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <process.h>
#include <pthread.h>
#include <stdlib.h>

#include "PCI_init.h"
#include "input.h"
#include "waveform.h"


int j;
float vert_offset;
char colon=':';
char argument;
char* argument_value;

float lower_limit = -5;
float upper_limit = 5;
float increment =0.1;

//variable for file logging
//variables for finding the duration that the program runs

pthread_t arrow_input_thread_ID;
pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;


void signal_handler( int signum)  //Ctrl+c handler
{
    //kill ncurses input as well
    pthread_cancel(arrow_input_thread_ID);
    pthread_mutex_lock(&mutex_wave_type);
    wave_type = ZERO;
    pthread_mutex_unlock(&mutex_wave_type);
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
    
    fp = fopen("savefile.txt","w");
    fprintf(fp,"%d\n%f\n%f\n%f\n%d\n",wave_type,amplitude,period,vert_offset,duty_cycle);
    fclose(fp);
    pci_detach_device(hdl);
    
    exit(EXIT_SUCCESS);                                 //exit the program
    
}

int main(int argc, char * argv[])
{
    //attach signal_handler to catch SIGINT
    signal(SIGINT, signal_handler);
    
    //Set Default Values of wave parameters
    wave_type = 0;    //sine
    vert_offset = 0;
    duty_cycle=50; 
	
	fp = fopen("savefile.txt","r");
	fscanf(fp,"%d %f %f %f %d", &prev_wave_type, &prev_amplitude, &prev_period, &prev_vert_offset, &prev_duty_cycle);
	vert_offset = prev_vert_offset;
	
    //command line argument(s)
    for(j=1;j<argc;j++)
    {
        //find colon
        if(argv[j][1]!=colon)   //invalid argument: colon not found at second character
        {
            printf("ERROR: Invalid command line argument\n");
            printf("Command line argument should be:\n");
            printf("./main t:wave_type v:offset\n");
            printf("*******************************************************\n");
            return 0;   //invalid, exit program
        }
        
        //get the argument (wave_type or vertical_offset)
        argument=argv[j][0];
        
        //get the VALUE of the argument
        argument_value=&(argv[j][2]);
        switch (argument)
        {
            case('v'):                                  //parse average/mean value and check whether it is of correct data type
                if(sscanf(argument_value,"%f",&vert_offset)!=1)
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Vertical offset must be FLOAT\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                } 
                else if(vert_offset<lower_limit || vert_offset>upper_limit)                //check if average is in valid range (-5 to 5)
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Invalid vert_offset\n");
                    printf("vert_offset must be between %.0f and %.0f\n",lower_limit,upper_limit);
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }    
                break;
                    
            case('t'):  
                if(sscanf(argument_value,"%d",&wave_type)!=1)   //parse wave type and check whether it is of correct data type
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: wave type must be INT (0,1,2,3)\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }
            else if(wave_type!=SINE && wave_type!=SQUARE && wave_type!=SAWTOOTH && wave_type!=TRIANGULAR)   //check if wave type value is valid
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Invalid input\n");
                    printf("Input 1 for sine wave, 2 for square wave, 3 for triangular wave, 0 for zero signal (no wave)\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }  
                break;
                
            default:    //invalid
                printf("\n*******************************************************\n");
                printf("ERR: Invalid command line argument\n");
                printf("Command line argument should be as:\n");
                printf("./main t:wave_type amplitude:A_value m:mean_value f:frequency level D:duty_cyctle_value\n");
                printf("*******************************************************\n");
                return 0;   //invalid, exit program
                break;
        }
    }
   
    initialization();   //initialize hardware
    initialize_DIO();   //initialize Digital Input Output
    Initialize_ADC();   //initialize ADC
    
    //get the time when the program starts
    if(clock_gettime(CLOCK_REALTIME,&start)==-1)
    { 
        printf("clock gettime start error");
    } 
    
    //create/open log.txt for logging & write starting message
    fp = fopen("log.txt","a");
    fprintf(fp,"Starting program\n");
    fclose(fp);
    
    pthread_create( &arrow_input_thread_ID, NULL, &arrow_input_thread, NULL );
    pthread_create( &hardware_input_thread_ID, NULL, &hardware_input_thread, NULL );
    pthread_create( &waveform_thread_ID, NULL, &waveform_thread, NULL );   
           
 	while(1)
 	{}
  	
  	printf("\n\nExit Program\n");
    pci_detach_device(hdl);
    
    return 0;
}


