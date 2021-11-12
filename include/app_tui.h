#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #define DEBUG

// --------------------------- Global variables ------------------------------
// Initialise windows
WINDOW* win_wave_plot;
WINDOW* win_description;
WINDOW* win_feedback;
WINDOW* win_toggle;

int i;
int j;
int k;
int win_wave_plot_height, win_wave_plot_width;
int cached_y_max, cached_x_max, y_max, x_max;
int win_panel_height, win_panel_width;
int x_padding, y_padding, key;
int graph_types_toggle_index = 0;
const char* graph_types_toggle[4];
  int points_len = 100;
<<<<<<< HEAD
  //float amplitude = 1;
  //float freq = 1.5;
  float freq;
=======
  float amplitude = 1;
  float freq = 1.5;
>>>>>>> [UNTESTED] Integrated app_tui with hardware
typedef struct _coordinate {
  int x;
  int y;
} Coordinate;
typedef enum _graphType {
  SINE = 0,
  SQUARE = 1,
  TRIANGULAR = 2,
  SAWTOOTH = 3
} GraphType;

  GraphType graph_type;
// Datapoint generation
Coordinate* GenerateCoordinates(GraphType type, int point_size, float amplitude,
                                float freq);
Coordinate* points;
// Drawing functions
void DrawAxes();
void DrawPoint(int point_x, int point_y);
void DrawPointSet(Coordinate* target_pair, int size);
void LocateLimits(Coordinate* target_pair, int size, int* x_lower_bound,
                  int* x_upper_bound, int* y_lower_bound, int* y_upper_bound);
void UpdateStats(float amplitude, float frequency);

int *app_tui_thread(void *arg); 