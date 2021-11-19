#ifndef TERMINAL_UI
#define TERMINAL_UI

#include <ncurses.h>
#include "common.h"

#define BASE_DELAY 1000
#define MAIN_TEXT_COLOUR 6

/* ------------------------------- Functions ---------------------------------- */
// Main TUI drawing loop
void DisplayTUI();  

// Function responsible for general TUI layout and design
void WindowDesign(WINDOW* win_wave_plot, WINDOW* win_description,
                  WINDOW* win_feedback, WINDOW* win_toggle);

// Functions for drawing and displaying wave plots
void DrawAxes(WINDOW* win, int win_wave_plot_height, int win_wave_plot_width,
              float vertical_offset);
void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, WaveType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width, int duty_cycle);
void PlotPoint(WINDOW* win, int x, int y);
void DisplayInactivePlot(WINDOW* win, int win_width, int win_height);

// Function responsible for updating wave statistics
void UpdateStats(WINDOW* win, float amplitude, float frequency,
                 float vert_offset, int duty_cycle, WaveType wave_type);
/* ---------------------------------------------------------------------------- */

#endif /* TERMINAL_UI */
