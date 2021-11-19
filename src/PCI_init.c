// PCI_init.c --> initialize PCI hardware

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>

#include "PCI_init.h"

void initialization()  // init PCI hardware
{
  memset(&info, 0, sizeof(info));
  if (pci_attach(0) < 0) {
    perror("pci_attach");
    exit(EXIT_FAILURE);
  }

  if (PCIe) {
    info.VendorId = 0x1307;  // Vendor and Device ID
    info.DeviceId = 0x115;
  }

  if (PCI) {
    info.VendorId = 0x1307;
    info.DeviceId = 0x01;
  }

  if ((hdl = pci_attach_device(0, PCI_SHARE | PCI_INIT_ALL, 0, &info)) == 0) {
    perror("pci_attach_device");
    exit(EXIT_FAILURE);
  }
  // Determine assigned BADRn IO addresses for PCI-DAS1602
  for (i = 0; i < 5; i++) {
    badr[i] = PCI_IO_ADDR(info.CpuBaseAddress[i]);
  }

  for (i = 0; i < 5;
       i++) {  // expect CpuBaseAddress to be the same as iobase for PC
    iobase[i] = mmap_device_io(0x0f, badr[i]);
  }
  // Modify thread control privity
  if (ThreadCtl(_NTO_TCTL_IO, 0) == -1) {
    perror("Thread Control");
    exit(1);
  }
}

void initialize_DIO()  // init digital I/O
{
#if PCI
  out8(DIO_CTLREG, 0x90);  // Port A : Input,  Port B : Output,  Port C (upper |
                           // lower) : Output | Output

#endif
}

void Initialize_ADC()  // init analog-digital converter
{
#if PCIe
  out8(CLK_Pace, 0x00);  // set to SW pacing & verify

  out8(ADC_Enable, 0x01);  // set bursting off, conversions on
  out8(ADC_Gain, 0x01);    // set range : 5V
  out8(MUXCHAN, 0x10);     // set mux for single channel scan : 1
#endif

#if PCI
  out16(TRIGGER, 0x2081);  // set trigger control
  out16(AUTOCAL, 0x007f);  // set calibration register
  out16(AD_FIFOCLR, 0);    // clear ADC FIFO
  out16(MUXCHAN, 0x0C10);  // select the channel
#endif
}
