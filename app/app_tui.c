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

int win_wave_plot_height, win_wave_plot_width;

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

// Datapoint generation
Coordinate* GenerateCoordinates(GraphType type, int point_size, float amplitude,
                                float freq);

// Drawing functions
void DrawAxes();
void DrawPoint(int point_x, int point_y);
void DrawPointSet(Coordinate* target_pair, int size);
void LocateLimits(Coordinate* target_pair, int size, int* x_lower_bound,
                  int* x_upper_bound, int* y_lower_bound, int* y_upper_bound);
void UpdateStats(float amplitude, float frequency);

int main(void) {
#ifndef DEBUG
  initscr(); /* Start curses mode */
  noecho();
  curs_set(0);
  keypad(stdscr, true);
#endif

  int cached_y_max, cached_x_max, y_max, x_max;
  int win_panel_height, win_panel_width;
  int x_padding, y_padding, key;
  int graph_types_toggle_index = 0;
  const char* graph_types_toggle[4];
  graph_types_toggle[0] = "SINE";
  graph_types_toggle[1] = "SQUARE";
  graph_types_toggle[2] = "TRIANGULAR";
  graph_types_toggle[3] = "SAW TOOTH";

  getmaxyx(stdscr, cached_y_max, cached_x_max);
  x_padding = 3;
  y_padding = 1;
  win_wave_plot_height = cached_y_max * 7 / 11;
  win_wave_plot_width = cached_x_max - 2 * x_padding;
  win_panel_height = cached_y_max * 4 / 11 - 1;
  win_panel_width = win_wave_plot_width / 3;

#ifndef DEBUG
  // Initialise windows
  win_wave_plot =
      newwin(win_wave_plot_height, win_wave_plot_width, y_padding, x_padding);
  box(win_wave_plot, 0, 0);

  win_description =
      newwin(win_panel_height, win_panel_width, y_padding, x_padding);
  box(win_description, 0, 0);
  mvwin(win_description, y_padding + win_wave_plot_height, x_padding);

  win_feedback =
      newwin(win_panel_height, win_panel_width, y_padding, x_padding);
  box(win_feedback, 0, 0);
  mvwin(win_feedback, y_padding + win_wave_plot_height,
        x_padding + win_panel_width);

  win_toggle = newwin(win_panel_height, win_panel_width, y_padding, x_padding);
  box(win_toggle, 0, 0);
  mvwin(win_toggle, y_padding + win_wave_plot_height,
        x_padding + 2 * win_panel_width);

  mvwprintw(win_wave_plot, 0, 2, " Wave Plot ");
  mvwprintw(win_description, 0, 2, " Description ");
  mvwprintw(win_feedback, 0, 2, " Statistics ");
  mvwprintw(win_toggle, 0, 2, " Controls ");
  mvwprintw(win_toggle, 2, 2, "Graph Type: SINE");

  DrawAxes();
#endif

  int points_len = 100;
  float amplitude = 1;
  float freq = 1.5;
  GraphType graph_type = SINE;
  Coordinate* points =
      GenerateCoordinates(graph_type, points_len, amplitude, freq);
  DrawPointSet(points, points_len);
#ifndef DEBUG
  while (key != 'q') {
    switch (key) {
      case KEY_RESIZE:
        getmaxyx(stdscr, y_max, x_max);

        // Check if window size has change - if yes, recalculate
        if (y_max != cached_y_max || x_max != cached_x_max) {
          cached_x_max = x_max;
          cached_y_max = y_max;
          win_wave_plot_height = cached_y_max * 7 / 11;
          win_wave_plot_width = cached_x_max - 2 * x_padding;
          win_panel_height = cached_y_max * 4 / 11 - 1;
          win_panel_width = win_wave_plot_width / 3;

          wresize(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
          wresize(win_description, win_panel_height, win_panel_width);
          wresize(win_feedback, win_panel_height, win_panel_width);
          wresize(win_toggle, win_panel_height, win_panel_width);

          mvwin(win_wave_plot, y_padding, x_padding);
          mvwin(win_description, y_padding + win_wave_plot_height, x_padding);
          mvwin(win_feedback, y_padding + win_wave_plot_height,
                x_padding + win_panel_width);
          mvwin(win_toggle, y_padding + win_wave_plot_height,
                x_padding + 2 * win_panel_width);

          // wclear(stdscr);
          wclear(win_wave_plot);
          wclear(win_description);
          wclear(win_feedback);
          wclear(win_toggle);

          DrawAxes();
          box(win_wave_plot, 0, 0);
          box(win_description, 0, 0);
          box(win_feedback, 0, 0);
          box(win_toggle, 0, 0);
          DrawPointSet(points, points_len);

          mvwprintw(win_wave_plot, 0, 2, " Wave Plot ");
          mvwprintw(win_description, 0, 2, " Description ");
          mvwprintw(win_feedback, 0, 2, " Statistics ");
          mvwprintw(win_toggle, 0, 2, " Controls ");
          mvwprintw(win_toggle, 2, 2, "Graph Type: ");
          mvwprintw(win_toggle, 2, 14,
                    graph_types_toggle[graph_types_toggle_index]);
          UpdateStats(amplitude, freq);
        }
        break;

      case KEY_LEFT:
        if (graph_types_toggle_index == 0) {
          graph_types_toggle_index = 3;
        } else {
          graph_types_toggle_index--;
        }
        graph_type = graph_types_toggle_index;
        points = GenerateCoordinates(graph_type, points_len, amplitude, freq);
        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes();
        box(win_wave_plot, 0, 0);
        box(win_toggle, 0, 0);
        DrawPointSet(points, points_len);
        mvwprintw(win_wave_plot, 0, 2, " Wave Plot ");
        mvwprintw(win_toggle, 0, 2, " Controls ");
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 14,
                  graph_types_toggle[graph_types_toggle_index]);
        break;
      case KEY_RIGHT:
        if (graph_types_toggle_index == 3) {
          graph_types_toggle_index = 0;
        } else {
          graph_types_toggle_index++;
        }
        graph_type = graph_types_toggle_index;
        points = GenerateCoordinates(graph_type, points_len, amplitude, freq);
        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes();
        box(win_wave_plot, 0, 0);
        box(win_toggle, 0, 0);
        DrawPointSet(points, points_len);
        mvwprintw(win_wave_plot, 0, 2, " Wave Plot ");
        mvwprintw(win_toggle, 0, 2, " Controls ");
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 14,
                  graph_types_toggle[graph_types_toggle_index]);
        break;
      default:
        break;
    }

    wrefresh(stdscr);
    wrefresh(win_wave_plot);
    wrefresh(win_description);
    wrefresh(win_feedback);
    wrefresh(win_toggle);
    key = getch();
  }

  endwin(); /* End curses mode */
#endif
  return 0;
}

void DrawAxes() {
  // draw y axis
  for (int i = 2; i < win_wave_plot_height - 2; i++) {
    mvwprintw(win_wave_plot, i, 5, "|");
  }

  // draw x axis
  for (int i = 2; i < win_wave_plot_width - 2; i++) {
    if (i == 5) {
      mvwprintw(win_wave_plot, win_wave_plot_height / 2, i, "0");
    } else {
      mvwprintw(win_wave_plot, win_wave_plot_height / 2, i, "-");
    }
  }

  // wrefresh(win_wave_plot);
}

void DrawPoint(int point_x, int point_y) {
  // if (point_x < win_wave_plot_width && point_y < win_wave_plot_height) {
  // Print and 'o' at the given coordinates
  mvwprintw(win_wave_plot, (win_wave_plot_height / 2) - point_y, 5 + point_x,
            ".");

  /*
  Print - for on x axis and | for y axis
  to 'trace' the point, like that

y|
  |--o
  |__|___x   */

  // for (int x = 5;
  //      point_x > 0 ? x < (win_wave_plot_width / 2) + point_x
  //                  : x > (win_wave_plot_width / 2) + point_x;
  //      // if x>0 while(x<point_x){x++} else while(x>point_x){x--}
  //      point_x > 0 ? x++ : x--) {
  //   mvwprintw(win_wave_plot, (win_wave_plot_height / 2) - point_y, x, "-");
  // }

  // for (int y = point_y > 0 ? (win_wave_plot_height / 2) - 1
  //                          : (win_wave_plot_height / 2);
  //      point_y > 0 ? y > (win_wave_plot_height / 2) - point_y
  //      : y<(win_wave_plot_height / 2) - point_y;
  //          // if y>0 while(y>point_y){y--} else while(y<point_y){y++}
  //          point_y> 0
  //          ? y--
  //          : y++) {
  //   mvwprintw(win_wave_plot, y, (win_wave_plot_width / 2) + point_x, "|");
  // }

  // Print point coordinates in absolute form on the relative axis
  // mvwprintw(win_wave_plot, (win_wave_plot_height / 2) - point_y,
  //           5, "%i",
  //           abs(point_y));  // y
  // mvwprintw(win_wave_plot, win_wave_plot_height / 2,
  //           5 + point_x, "%i",
  //           abs(point_x));  // x

  // wrefresh(win_wave_plot);
  // }
}

void DrawPointSet(Coordinate* target_pair, int size) {
  int x_lower_bound = 0;
  int x_upper_bound = 0;
  int y_lower_bound = 0;
  int y_upper_bound = 0;

  LocateLimits(target_pair, size, &x_lower_bound, &x_upper_bound,
               &y_lower_bound, &y_upper_bound);
  // printf("x_lower_bound: %d, x_upper_bound: %d\n", x_lower_bound,
  //        x_upper_bound);
  // printf("y_lower_bound: %d, y_upper_bound: %d\n", y_lower_bound,
  //        y_upper_bound);
  // printf("Width: %d, Height: %d\n", win_wave_plot_width,
  // win_wave_plot_height);

  for (int i = 0; i < size; ++i) {
    // printf("og x: %d, og y: %d\n", target_pair[i].x, target_pair[i].y);
    // Scale first
    int x, y;
    x = target_pair[i].x * (win_wave_plot_width - 10) /
        ((x_upper_bound - x_lower_bound));
    y = target_pair[i].y * (win_wave_plot_height / 1.8 - 2.0) /
        ((y_upper_bound - y_lower_bound));
// x = target_pair[i].x;
// y = target_pair[i].y;

// printf("x: %d, y: %d\n", x, y);
#ifndef DEBUG
    DrawPoint(x, y);
#endif
  }
}

void LocateLimits(Coordinate* target_pair, int size, int* x_lower_bound,
                  int* x_upper_bound, int* y_lower_bound, int* y_upper_bound) {
  for (int i = 0; i < size; ++i) {
    if (target_pair[i].x > *x_upper_bound) {
      *x_upper_bound = target_pair[i].x;
    } else if (target_pair[i].x < *x_lower_bound) {
      *x_lower_bound = target_pair[i].x;
    }
    if (target_pair[i].y > *y_upper_bound) {
      *y_upper_bound = target_pair[i].y;
    } else if (target_pair[i].y < *y_lower_bound) {
      *y_lower_bound = target_pair[i].y;
    }
  }
}

Coordinate* GenerateCoordinates(GraphType type, int point_size, float amplitude,
                                float frequency) {
  Coordinate* coords = (Coordinate*)malloc(point_size * sizeof(Coordinate));
  switch (type) {
    case SINE:
      for (int i = 0; i < point_size; ++i) {
        coords[i].x = i;
        coords[i].y = amplitude *
                      sin(frequency * ((double)i / point_size) * (2 * M_PI)) *
                      100;
      }
      break;
    case SQUARE: {
      int step_size;
      float x_proportion;
      step_size = (float)point_size / (float)frequency - 2;
      for (int i = 0; i < step_size; ++i) {
        x_proportion = (float)i / (float)step_size;
        coords[i].x = i;

        if (x_proportion < 0.5) {
          coords[i].y = amplitude * 15;
        } else {
          coords[i].y = amplitude * -15;
        }
        // printf("proportion: %.3f, x: %d, y: %d\n", x_proportion,
        // coords[i].x, coords[i].y);
      }
      int k = 0;
      for (int j = step_size; j < (step_size + ((frequency - 1) * step_size));
           ++j) {
        coords[j].x = j;
        coords[j].y = coords[k].y;
        k++;
      }
      break;
    }
    case TRIANGULAR: {
      int cached_x1, cached_x2, cached_y, step_size;
      float x_proportion;
      step_size = (float)point_size / (float)frequency - 2;

      for (int i = 0; i < step_size; ++i) {
        x_proportion = (float)i / (float)step_size;
        coords[i].x = i;

        if (x_proportion < 0.25) {
          coords[i].y = i;
          cached_x1 = i;
        } else if (x_proportion < 0.75) {
          coords[i].y = (cached_x1 - (i - cached_x1));
          cached_y = coords[i].y;
          cached_x2 = i;
        } else {
          coords[i].y = (cached_y + (i - cached_x2));
        }
        // printf("proportion: %.3f, x: %d, y: %d\n", x_proportion,
        // coords[i].x, coords[i].y);
      }
      int k = 0;
      for (int j = step_size; j < (step_size + ((frequency - 1) * step_size));
           ++j) {
        coords[j].x = j;
        coords[j].y = coords[k].y;
        k++;
      }

      break;
    }
    case SAWTOOTH: {
      int cached_y, step_size, step_count;
      float x_proportion;
      step_count = frequency * 2;
      step_size = (float)point_size / (float)step_count - 2;

      for (int i = 0; i < step_size; ++i) {
        x_proportion = (float)i / (float)step_size;
        coords[i].x = i;
        coords[i].y = 10 * (float)i * amplitude / (float)step_size;
        // printf("x: %d, y: %d\n", coords[i].x, coords[i].y);
      }
      int k = 0;
      for (int j = step_size; j < (step_size + ((step_count - 1) * step_size));
           ++j) {
        coords[j].x = j;
        coords[j].y = coords[k].y;
        k++;
      }

      break;
    }
    default:
      break;
  }
  UpdateStats(amplitude, frequency);
  return coords;
}

void UpdateStats(float amplitude, float frequency) {
  float period;
  period = 1 / frequency;
  mvwprintw(win_feedback, 2, 2, "Amplitude: ");
  mvwprintw(win_feedback, 3, 2, "Frequency: ");
  mvwprintw(win_feedback, 4, 2, "Period: ");
  mvwprintw(win_feedback, 2, 13, "%.3f", amplitude);
  mvwprintw(win_feedback, 3, 13, "%.3f Hz", frequency);
  mvwprintw(win_feedback, 4, 13, "%.3f seconds", period);
}