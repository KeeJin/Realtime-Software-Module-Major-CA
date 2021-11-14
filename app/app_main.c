#include "terminal_ui.h"

// #include <stdio.h>
#include <stdlib.h>

int main(void) {
  pthread_t display_thread;
  pthread_attr_t attr;
  int rc;
  long t;
  void* status;

  /* ------------------- Adjustable params ------------------- */
  pthread_mutex_lock(&mutex);
  time_period_ms = 150;
  graph_type = SINE;
  amplitude = 3.0;
  frequency = 5.0;
  vertical_offset = -2.0;
  phase_shift = 0.0;
  pthread_mutex_unlock(&mutex);
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

