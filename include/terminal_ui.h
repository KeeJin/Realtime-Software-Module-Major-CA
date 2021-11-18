#ifndef TERMINAL_UI
#define TERMINAL_UI

#include <ncurses.h>
#include "common.h"

#define BASE_DELAY 1000
#define MAIN_TEXT_COLOUR 6


/* ------------------------------- Functions ---------------------------------- */
void DisplayTUI();  // drawing thread
void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, WaveType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width, int duty_cycle);
void PlotPoint(WINDOW* win, int x, int y);
void WindowDesign(WINDOW* win_wave_plot, WINDOW* win_description,
                  WINDOW* win_feedback, WINDOW* win_toggle);
void DrawAxes(WINDOW* win, int win_wave_plot_height, int win_wave_plot_width,
              float vertical_offset);
void DrawPoint(WINDOW* win, int point_x, int point_y);
void UpdateStats(WINDOW* win, float amplitude, float frequency,
                 float vert_offset, int duty_cycle, WaveType wave_type);
void DisplayInactivePlot(WINDOW* win, int win_width, int win_height);
/* ---------------------------------------------------------------------------- */

#endif /* TERMINAL_UI */
