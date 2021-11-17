// waveform.c --> waveform functions are here
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

int N = 50;  // number of "cuts" to make wave

void sine_wave(unsigned int dio_switch_local, WaveType wave_type_local, 
                float amplitude_local,float period_local, 
                float vert_offset_local, int duty_cycle_local, int prev_switch0)  // sine wave function
{
  int beeping = 1;  // to allow for only 1 beep per peak (for sine)
  while ((wave_type_local == 0) &&
         (switch3_value(dio_switch_local)))  // stops if wave_type is not 1 (sine)
  {
    for (i = 0; i < N; i++) {
      if (!(wave_type_local == 0) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
#if PCI
      data = (((sinf((float)(i * 2 * 3.1415 / N)) * amplitude_local) + 10 +
               vert_offset_local) /
              20.0 * 0xFFFF);  // send code here
      out16(DA_CTLREG,
            0x0923);         // DA Enable, #0, #1, SW 10V bipolar		2/6
      out16(DA_FIFOCLR, 0);  // Clear DA FIFO  buffer
      out16(DA_Data, (short)data);
#endif

#if PCIe
      data = (((sinf((float)(i * 2 * 3.1415 / N)) * amplitude_local) + 10 +
               vert_offset_local) /
              20.0 * 0x0FFF);  // send code here
      out16(DAC0_Data, data);
#endif

      delay((int)period_local);
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);

      if ((sinf((float)(i * 2 * 3.1415 / N)) >= 0.99) &&
          beeping)  // beep if wave reaches max peak
      {
        // printf("%.2f", sinf((float)(i*2*3.1415/N)));
        if (switch1_value(dio_switch_local)) putchar(7);
        printf("\n");
        beeping = !beeping;
      }

      else if ((sinf((float)(i * 2 * 3.1415 / N)) <= -0.99) &&
               !beeping)  // beep if wave reaches min peak
      {
        //	printf("%.2f", sinf((float)(i*2*3.1415/N)));
        if (switch1_value(dio_switch_local)) putchar(7);
        printf("\n");
        beeping = !beeping;
      }
    }
  }
}

void square_wave(unsigned int dio_switch_local, WaveType wave_type_local, 
                  float amplitude_local,float period_local, 
                  float vert_offset_local, int duty_cycle_local, int prev_switch0)  // square wave function
{
  while ((wave_type_local == 1) &&
         (switch3_value(dio_switch_local)))  // stops if wave_type is not 2 (square)
  {
    if (switch1_value(dio_switch_local))
      putchar(7);  // beeps at first time wave reaches high for the cycle
    printf("\n");

#if PCI
    data = ((amplitude_local + 10 + vert_offset_local) / 20.0 * 0xFFFF);
    out16(DA_CTLREG,
          0x0923);         // DA Enable, #0, #1, SW 10V bipolar		2/6
    out16(DA_FIFOCLR, 0);  // Clear DA FIFO  buffer
    out16(DA_Data, (short)data);
#endif

#if PCIe
    data = ((amplitude_local + 10 + vert_offset_local) / 20.0 * 0x0FFF);
    out16(DAC0_Data, data);
#endif

    for (i = 0; i < N * duty_cycle_local / 100; i++) {
      if (!(wave_type_local == 1) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
      delay((int)period_local);
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }

    if (switch1_value(dio_switch_local)) putchar(7);
    printf("\n");  // beeps at first time wave reaches low for the cycle

#if PCI
    data = ((10 - amplitude_local + vert_offset_local) / 20.0 * 0xFFFF);
    out16(DA_CTLREG,
          0x0923);         // DA Enable, #0, #1, SW 10V bipolar		2/6
    out16(DA_FIFOCLR, 0);  // Clear DA FIFO  buffer
    out16(DA_Data, (short)data);
#endif

#if PCIe
    data = ((10 - amplitude_local + vert_offset_local) / 20.0 * 0x0FFF);
    out16(DAC0_Data, data);
#endif

    for (i = 0; i < N * (100 - duty_cycle_local) / 100; i++) {
      if (!(wave_type_local == 1) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
      delay((int)period_local);
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }
  }
}

void triangular_wave(unsigned int dio_switch_local, WaveType wave_type_local, 
                      float amplitude_local,float period_local, 
                      float vert_offset_local, int duty_cycle_local, int prev_switch0) 
{
  while (
      (wave_type_local == 2) &&
      (switch3_value(dio_switch_local)))  // stops if wave_type is not 3 (triangular)
  {
    for (i = 0; i < N / 2; i++) {
      if (!(wave_type_local == 2) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
#if PCI
      data = ((vert_offset_local - amplitude_local + amplitude_local * 4 * i / N + 10) / 20.0 *
              0xFFFF);
      out16(DA_CTLREG, 0x0923);  // DA Enable, #0, #1, SW 10V bipolar
      out16(DA_FIFOCLR, 0);      // Clear DA FIFO  buffer
      out16(DA_Data, (short)data);
#endif

#if PCIe
      data = ((vert_offset_local - amplitude_local + amplitude_local * 4 * i / N + 10) / 20.0 *
              0x0FFF);
      out16(DAC0_Data, data);
#endif

      delay((int)period_local);  // micros
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }
    if (switch1_value(dio_switch_local)) putchar(7);  // Beeps at peak
    printf("\n");

    for (i = 0; i < N / 2; i++) {
      if (!(wave_type_local == 2) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
#if PCI
      data = ((vert_offset_local + amplitude_local - amplitude_local * 4 * i / N + 10) / 20.0 *
              0xFFFF);
      out16(DA_CTLREG, 0x0923);  // DA Enable, #0, #1, SW 10V bipolar
      out16(DA_FIFOCLR, 0);      // Clear DA FIFO  buffer
      out16(DA_Data, (short)data);
#endif

#if PCIe
      data = ((vert_offset_local + amplitude_local - amplitude_local * 4 * i / N + 10) / 20.0 *
              0x0FFF);
      out16(DAC0_Data, data);
#endif

      delay((int)period_local);  // micros
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }
    if (switch1_value(dio_switch_local)) putchar(7);  // Beeps at min
    printf("\n");
  }
}
void sawtooth_wave(unsigned int dio_switch_local, WaveType wave_type_local, 
                    float amplitude_local,float period_local, 
                    float vert_offset_local, int duty_cycle_local, int prev_switch0) 
{
  while (
      (wave_type_local == 3) &&
      (switch3_value(dio_switch_local)))  // stops if wave_type is not 3 (triangular)
  {
    for (i = 0; i < N; i++) {
      if (!(wave_type_local == 3) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
#if PCI
      data = ((vert_offset_local - amplitude_local + amplitude_local * 2 * i / N + 10) / 20.0 *
              0xFFFF);
      out16(DA_CTLREG, 0x0923);  // DA Enable, #0, #1, SW 10V bipolar
      out16(DA_FIFOCLR, 0);      // Clear DA FIFO  buffer
      out16(DA_Data, (short)data);
#endif

#if PCIe
      data = ((vert_offset_local - amplitude_local + amplitude_local * 2 * i / N + 10) / 20.0 *
              0x0FFF);
      out16(DAC0_Data, data);
#endif

      delay((int)period_local);  // micros
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }
    if (switch1_value(dio_switch_local)) putchar(7);  // Beeps at peak
    printf("\n");

  }  // sawtooth wave here
}

void zero_signal(unsigned int dio_switch_local, WaveType wave_type_local, 
                  float amplitude_local,float period_local, 
                  float vert_offset_local, int duty_cycle_local, int prev_switch0) 
{
#if PCI
  // data= (5 + vert_offset)/10* 0xFFFF;
  data = 1 / 2 * 0xFFFF;     // corresponds to 0 voltage signal
  out16(DA_CTLREG, 0x0923);  // DA Enable, #0, #1, SW 10V bipolar
  out16(DA_FIFOCLR, 0);      // Clear DA FIFO  buffer
  out16(DA_Data, (short)data);
#endif

#if PCIe
  // data= (5 + vert_offset)/10* 0x0FFF;
  data = 1 / 2 * 0x0FFF;  // corresponds to 0 voltage signal
  out16(DAC0_Data, data);
#endif
  while ((wave_type_local == 4) ||
         !(switch3_value(
             dio_switch_local)))  // stops if wave_type is not 0 (zero voltage)
  {
    for (i = 0; i < N; i++) {
      if (!(wave_type_local == 4) || !(switch3_value(dio_switch_local)) || (switch0_value(dio_switch_local)!=prev_switch0) ) return;
#if PCI
      // data= (5 + vert_offset)/10* 0xFFFF;
      data = 1 / 2 * 0xFFFF;     // corresponds to 0 voltage signal
      out16(DA_CTLREG, 0x0923);  // DA Enable, #0, #1, SW 10V bipolar
      out16(DA_FIFOCLR, 0);      // Clear DA FIFO  buffer
      out16(DA_Data, (short)data);
#endif

#if PCIe
      // data= (5 + vert_offset)/10* 0x0FFF;
      data = 1 / 2 * 0x0FFF;  // corresponds to 0 voltage signal
      out16(DAC0_Data, data);
#endif

      delay((int)period_local);  // micros
      pthread_mutex_lock(&mutex_common);
      amplitude_local = amplitude;
      period_local = period;
      dio_switch_local = dio_switch;
      pthread_mutex_unlock(&mutex_common);

      pthread_mutex_lock(&mutex_wave_type);
      wave_type_local = wave_type;
      pthread_mutex_unlock(&mutex_wave_type);

      // MUTEX LOCK HERE
      duty_cycle_local = duty_cycle;
      // MUTEX UNLOCK HERE

      pthread_mutex_lock(&mutex_vertical_offset);
      vert_offset_local = vertical_offset;
      pthread_mutex_unlock(&mutex_vertical_offset);
    }
  }
}

void *waveform_thread(
    void *arg)  // thread to generate wave based on wave parameters
{
  unsigned int dio_switch_local;
  WaveType wave_type_local;
  float amplitude_local;
  float period_local;
  float vert_offset_local;
  int duty_cycle_local;
  int prev_switch0;

  pthread_mutex_lock(&mutex_common);
  amplitude_local = amplitude;
  period_local = period;
  dio_switch_local = dio_switch;
  pthread_mutex_unlock(&mutex_common);

  pthread_mutex_lock(&mutex_wave_type);
  wave_type_local = wave_type;
  pthread_mutex_unlock(&mutex_wave_type);

  // MUTEX LOCK HERE
  duty_cycle_local = duty_cycle;
  // MUTEX UNLOCK HERE

  pthread_mutex_lock(&mutex_vertical_offset);
  vert_offset_local = vertical_offset;
  pthread_mutex_unlock(&mutex_vertical_offset);


  prev_switch0 = switch0_value(dio_switch_local);

  while (switch0_value(dio_switch_local)==prev_switch0) {
    pthread_mutex_lock(&mutex_common);
    amplitude_local = amplitude;
    period_local = period;
    dio_switch_local = dio_switch;
    pthread_mutex_unlock(&mutex_common);

    pthread_mutex_lock(&mutex_wave_type);
    wave_type_local = wave_type;
    pthread_mutex_unlock(&mutex_wave_type);

    // MUTEX LOCK HERE
    duty_cycle_local = duty_cycle;
    // MUTEX UNLOCK HERE

    pthread_mutex_lock(&mutex_vertical_offset);
    vert_offset_local = vertical_offset;
    pthread_mutex_unlock(&mutex_vertical_offset);
    
    if (switch3_value(dio_switch_local)) {
      switch (wave_type_local) {
        case (SINE):  // sine
          sine_wave(dio_switch_local, wave_type_local, amplitude_local,
                    period_local, vert_offset_local, duty_cycle_local, prev_switch0);
          break;
        case (SQUARE):  // square
          square_wave(dio_switch_local, wave_type_local, amplitude_local,
                      period_local, vert_offset_local, duty_cycle_local, prev_switch0);
          break;
        case (TRIANGULAR):  // triangular
          triangular_wave(dio_switch_local, wave_type_local, amplitude_local,
                          period_local, vert_offset_local, duty_cycle_local, prev_switch0);
          break;
        case (SAWTOOTH):  // sawtooth
          sawtooth_wave(dio_switch_local, wave_type_local, amplitude_local,
                        period_local, vert_offset_local, duty_cycle_local, prev_switch0);
          break;
        case (ZERO):  // zero voltage
          zero_signal(dio_switch_local, wave_type_local, amplitude_local,
                      period_local, vert_offset_local, duty_cycle_local, prev_switch0);
          break;
        default:
          printf("invalid wave type error\n");
          break;
      }
    } else
      zero_signal(dio_switch_local, wave_type_local, amplitude_local,
                  period_local, vert_offset_local, duty_cycle_local, prev_switch0);
  }
}
