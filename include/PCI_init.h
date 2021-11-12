//headerfile for PCI_init.c

#ifndef __PCI_INIT_H
#define __PCI_INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>

#define PCIe		1		// 0: PCI 1: PCIe 
#define PCI 		!PCIe

// ********************************************************************
// PCIe Registers *****************************************************

#if PCIe				// define registers for PCIe board
#define 	INTERRUPT	iobase[1] + 4	// Badr1 + 4 - PCIe 32-bit

#define 	ADC_Data	iobase[2] + 0	// Badr2 + 0 - PCIe 16-bit w->srt
#define 	DAC0_Data	iobase[2] + 2	// Badr2 + 2 - PCIe 12-bit
#define 	DAC1_Data	iobase[2] + 4	// Badr2 + 4 - PCIe 12-bit
	
#define 	MUXCHAN		iobase[3] + 0	// Badr3 + 0 - Mux scan/upper lower
#define 	DIO_Data	iobase[3] + 1	// Badr3 + 1 - 8bit DI3 DI2 DI1 DI0
#define	ADC_Stat1		iobase[3] + 2 	// Badr3 + 2 - 1 : MSB
#define 	ADC_Stat2	iobase[3] + 3	// Badr3 + 3 - Alt EOC
#define	CLK_Pace		iobase[3] + 5	// Badr3 + 5 - S/W Pacer : XXXX XX0X
#define 	ADC_Enable	iobase[3] + 6	// Badr3 + 6 - Brst_off Conv_EN:0x01
#define 	ADC_Gain	iobase[3] + 7	// Badr3 + 7 - unipolar 5V : 0x01
#else 

// ********************************************************************
// PCI Registers - define registers for PCI board *********************


#define	INTERRUPT		iobase[1] + 0				// Badr1 + 0 : also ADC register
#define	MUXCHAN			iobase[1] + 2				// Badr1 + 2
#define	TRIGGER			iobase[1] + 4				// Badr1 + 4
#define	AUTOCAL			iobase[1] + 6				// Badr1 + 6
#define 	DA_CTLREG		iobase[1] + 8				// Badr1 + 8

#define	 AD_DATA			iobase[2] + 0				// Badr2 + 0
#define	 AD_FIFOCLR		iobase[2] + 2				// Badr2 + 2

#define	TIMER0				iobase[3] + 0				// Badr3 + 0
#define	TIMER1				iobase[3] + 1				// Badr3 + 1
#define	TIMER2				iobase[3] + 2				// Badr3 + 2
#define	COUNTCTL			iobase[3] + 3				// Badr3 + 3
#define	DIO_PORTA		iobase[3] + 4				// Badr3 + 4
#define	DIO_PORTB		iobase[3] + 5				// Badr3 + 5
#define	DIO_PORTC		iobase[3] + 6				// Badr3 + 6
#define	DIO_CTLREG		iobase[3] + 7				// Badr3 + 7
#define	PACER1				iobase[3] + 8				// Badr3 + 8
#define	PACER2				iobase[3] + 9				// Badr3 + 9
#define	PACER3				iobase[3] + a				// Badr3 + 0xa
#define	PACERCTL			iobase[3] + b				// Badr3 + 0xb

#define 	DA_Data			iobase[4] + 0				// Badr4 + 0
#define	DA_FIFOCLR		iobase[4] + 2				// Badr4 + 2
#endif



int badr[5];
struct pci_dev_info info;
void *hdl;
uintptr_t iobase[6];

uintptr_t dio_switch;
int switch0;
int switch0_prev;
unsigned int i;

void initialization();
void initialize_DIO();
void Initialize_ADC();


#endif /*__PCI_INIT_H__*/
