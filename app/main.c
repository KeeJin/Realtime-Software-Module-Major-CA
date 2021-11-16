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
#include "terminal_ui.h"


int j;
char colon=':';
char argument;
char* argument_value;

//variable for file logging
//variables for finding the duration that the program runs

pthread_t hardware_input_thread_ID;
pthread_t waveform_thread_ID;
pthread_t DisplayTUI_ID;


void signal_handler( int signum)  //Ctrl+c handler
{
    pthread_cancel(DisplayTUI_ID);
    endwin();
    system("clear");    
    if(current_period == 0)
    {   
        current_wave_type = prev_wave_type;
        current_amplitude = prev_amplitude;
        current_period = prev_period;
        current_vert_offset = prev_vert_offset;
    }
    fp = fopen("savefile.txt","w");
            fprintf(fp,"%d\n%f\n%f\n%f\n%d\n",current_wave_type,current_amplitude,current_period,current_vert_offset,duty_cycle);
    fclose(fp);
    pci_detach_device(hdl);
    printf("Ending program...\n");
    printf("Resetting hardware...\n");
    wave_type = ZERO;
    delay(period);
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

int main(int argc, char * argv[])
{
    pthread_t display_thread;
    pthread_attr_t attr;
    int rc;
    long t;
    void* status;

    //attach signal_handler to catch SIGINT
    signal(SIGINT, signal_handler);
    
    //Set Default Values of wave parameters
    wave_type = SINE;
    vertical_offset = 0.0;
    duty_cycle=50;
    period = 50.0;
	
	fp = fopen("savefile.txt","r");
	if(fp) fscanf(fp,"%d %f %f %f %d", &prev_wave_type, &prev_amplitude, &prev_period, &prev_vert_offset, &prev_duty_cycle);
	else
	{
		 prev_wave_type = SINE; 
		 prev_amplitude = 5.0;
		 prev_period = 50.0;
		 prev_vert_offset = 0.0;
		 prev_duty_cycle = 50;
	}
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
                if(sscanf(argument_value, "%f", &vertical_offset) != 1)
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Vertical offset must be FLOAT\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                } 
                else if(vertical_offset < LOWER_LIMIT_VOLTAGE || vertical_offset > UPPER_LIMIT_VOLTAGE)                //check if average is in valid range (-5 to 5)
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: Invalid vertical_offset\n");
                    printf("vertical_offset must be between %.0f and %.0f\n", LOWER_LIMIT_VOLTAGE, UPPER_LIMIT_VOLTAGE);
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }    
                break;
                    
            case('t'):  
                if(sscanf(argument_value, "%d", &wave_type) != 1)   //parse wave type and check whether it is of correct data type
                {
                    printf("\n*******************************************************\n");
                    printf("ERR: wave type must be INT (0,1,2,3)\n");
                    printf("*******************************************************\n");
                    return 0;   //invalid, exit program
                }
            else if(wave_type != 1 && wave_type != 2 && wave_type !=3 && wave_type!=0)   //check if wave type value is valid
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
    


    /* ------------------- Adjustable params ------------------- */
  //  pthread_mutex_lock(&mutex);
    time_period_ms = 50;
    frequency = 1.0/period;
    phase_shift = 0.0;
    //pthread_mutex_unlock(&mutex);
    /* ---------------------------------------------------------- */

    /* ----- Initialize and set thread detached attribute ------- */
    pthread_create(&DisplayTUI_ID, NULL, DisplayTUI, NULL);
    pthread_create( &hardware_input_thread_ID, NULL, &hardware_input_thread, NULL );
    pthread_create( &waveform_thread_ID, NULL, &waveform_thread, NULL );   
           
 	while(1)
 	{}
  	
  	printf("\n\nExit Program\n");
    pci_detach_device(hdl);
    
    return 0;
}


