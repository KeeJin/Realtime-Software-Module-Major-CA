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
#include "app_tui.h"


int j;
float vert_offset;
char colon=':';
char argument;
char* argument_value;



void signal_handler( int signum)  //Ctrl+c handler
{
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

	fp = fopen("prev_wave.txt","r");
	fscanf(fp, "%d %f %f %f %d",&prev_wave_type,&prev_amplitude,&prev_period,&prev_vert_offset,&prev_duty_cycle);  

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
            else if(wave_type!=1 && wave_type!=2 && wave_type!=3 && wave_type!=0)   //check if wave type value is valid
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Invalid input\n");
                    printf("Input 0 for sine wave, 1 for square wave, 2 for triangular wave, 3 for sawtooth wave\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }  
                break;
                
            default:    //invalid
                printf("\n*******************************************************\n");
                printf("ERR: Invalid command line argument\n");
                printf("Command line argument should be:\n");
                printf("../main t:wave_type v:offset\n");
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

    
    pthread_create( &arrow_input_thread_ID, NULL, &arrow_input_thread, NULL );
    pthread_create( &hardware_input_thread_ID, NULL, &hardware_input_thread, NULL );
    pthread_create( &app_tui_thread_ID, NULL, &app_tui_thread, NULL );
    
    delay(100);
    pthread_create( &waveform_thread_ID, NULL, &waveform_thread, NULL );   
           
 	while(1)
 	{
        if(switch2_value(dio_switch))
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
        else vert_offset = prev_vert_offset;

	}
  	
  	printf("\n\nExit Program\n");
    pci_detach_device(hdl);
    
    return 0;
}


