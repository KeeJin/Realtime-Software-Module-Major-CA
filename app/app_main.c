#include "terminal_ui.h"

// #include <stdio.h>
#include <stdlib.h>


pthread_mutex_t mutex_common;
pthread_mutex_t mutex_vertical_offset;
pthread_mutex_t mutex_wave_type;

int main(void) {
  pthread_t display_thread;
  pthread_attr_t attr;
  int rc;
  long t;
  void* status;

  /* ------------------- Adjustable params ------------------- */
  pthread_mutex_lock(&mutex_common);
  time_period_ms = 100;
  amplitude = 3.0;
  period = 50.0;
  pthread_mutex_unlock(&mutex_common);

  pthread_mutex_lock(&mutex_vertical_offset);
  vertical_offset = 0.0;
  pthread_mutex_unlock(&mutex_vertical_offset);

  pthread_mutex_lock(&mutex_wave_type);
  wave_type = SINE;
  pthread_mutex_unlock(&mutex_wave_type);
  /* ---------------------------------------------------------- */

  /* ----- Initialize and set thread detached attribute ------- */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  rc = pthread_create(&display_thread, &attr, DisplayTUI, (void*)t);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
  /* ---------------------------------------------------------- */

  /* ----- Free attribute and wait for the other threads ------ */
  pthread_attr_destroy(&attr);
  rc = pthread_join(display_thread, &status);
  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
  /* ---------------------------------------------------------- */

  pthread_exit(NULL);
  return 0;
}

