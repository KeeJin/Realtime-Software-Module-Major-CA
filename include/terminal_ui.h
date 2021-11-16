#ifndef TERMINAL_UI
#define TERMINAL_UI

#include <ncurses.h>
#include <pthread.h>

#define BASE_DELAY 1000
#define MAIN_TEXT_COLOUR 6

#define HARDWARE 1 //0: without hardware; 1: with hardware 

typedef enum _graphType {
  SINE = 0,
  SQUARE = 1,
  TRIANGULAR = 2,
  SAWTOOTH = 3
} GraphType;

/* ----------------------------- Global params -------------------------------- */
GraphType graph_type;
float amplitude;
float frequency;
float phase_shift;
float vertical_offset;
int time_period_ms;
extern pthread_mutex_t mutex;
/* ---------------------------------------------------------------------------- */

/* ------------------------------- Functions ---------------------------------- */
void* DisplayTUI(void* arg);  // drawing thread
void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, GraphType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width);
void PlotPoint(WINDOW* win, int x, int y);
void WindowDesign(WINDOW* win_wave_plot, WINDOW* win_description,
                  WINDOW* win_feedback, WINDOW* win_toggle);
void DrawAxes(WINDOW* win, int win_wave_plot_height, int win_wave_plot_width,
              float vertical_offset);
void DrawPoint(WINDOW* win, int point_x, int point_y);
void UpdateStats(WINDOW* win, float amplitude, float frequency, float vert_offset);
/* ---------------------------------------------------------------------------- */

#endif /* TERMINAL_UI */
