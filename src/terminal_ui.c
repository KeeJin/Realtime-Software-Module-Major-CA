#include "terminal_ui.h"

#include <math.h>
#include <unistd.h>

pthread_mutex_t mutex;

void* DisplayTUI(void* args) {
  WINDOW* win_wave_plot;
  WINDOW* win_description;
  WINDOW* win_feedback;
  WINDOW* win_toggle;
  int win_wave_plot_height, win_wave_plot_width;
  int cached_y_max, cached_x_max, y_max, x_max;
  int win_panel_height, win_panel_width;
  int x_padding, y_padding, key;
  int graph_types_toggle_index = 0;
  float scaled_amplitude;
  GraphType graph_type_local;
  float amplitude_local;
  float frequency_local;
  float phase_shift_local;
  int time_period_ms_local;
  const char* graph_types_toggle[4];

#ifndef DEBUG
  initscr(); /* Start curses mode */
  start_color();
  nodelay(stdscr, 1);
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, 1);

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);

#endif

  graph_types_toggle[0] = "SINE";
  graph_types_toggle[1] = "SQUARE";
  graph_types_toggle[2] = "TRIANGULAR";
  graph_types_toggle[3] = "SAW TOOTH";

  getmaxyx(stdscr, cached_y_max, cached_x_max);
  x_padding = 1;
  y_padding = 1;
  win_wave_plot_height = cached_y_max * 7 / 11;
  win_wave_plot_width = cached_x_max - (2 * x_padding);
  win_panel_height = cached_y_max * 4 / 11 - 1;
  win_panel_width = win_wave_plot_width / 3;

  scaled_amplitude = 0.8 * (win_wave_plot_height / 2);

#ifndef DEBUG
  // Initialise windows
  win_wave_plot =
      newwin(win_wave_plot_height, win_wave_plot_width, y_padding, x_padding);

  win_description =
      newwin(win_panel_height, win_panel_width, y_padding, x_padding);
  mvwin(win_description, y_padding + win_wave_plot_height, x_padding);

  win_feedback =
      newwin(win_panel_height, win_panel_width, y_padding, x_padding);

  mvwin(win_feedback, y_padding + win_wave_plot_height,
        x_padding + win_panel_width);

  win_toggle = newwin(win_panel_height, win_panel_width, y_padding, x_padding);

  mvwin(win_toggle, y_padding + win_wave_plot_height,
        x_padding + 2 * win_panel_width);

  WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
  wattron(win_toggle, A_BOLD);
  wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win_toggle, 2, 2, "Graph Type:   SINE");
  wattroff(win_toggle, A_BOLD);

  DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
#endif
  pthread_mutex_lock(&mutex);
  graph_type_local = graph_type;
  amplitude_local = amplitude;
  frequency_local = frequency;
  phase_shift_local = phase_shift;
  time_period_ms_local = time_period_ms;
  pthread_mutex_unlock(&mutex);
  PlotGraph(win_wave_plot, win_feedback, graph_type, amplitude,
            scaled_amplitude, frequency, phase_shift, win_wave_plot_height,
            win_wave_plot_width);
#ifndef DEBUG
  while (key != 'q') {
    pthread_mutex_lock(&mutex);
    graph_type_local = graph_type;
    amplitude_local = amplitude;
    frequency_local = frequency;
    phase_shift_local = phase_shift;
    time_period_ms_local = time_period_ms;
    pthread_mutex_unlock(&mutex);
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

          wclear(win_wave_plot);
          wclear(win_description);
          wclear(win_feedback);
          wclear(win_toggle);

          DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
          WindowDesign(win_wave_plot, win_description, win_feedback,
                       win_toggle);
          wattron(win_toggle, A_BOLD);
          wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
          mvwprintw(win_toggle, 2, 2, "Graph Type: ");
          mvwprintw(win_toggle, 2, 16,
                    graph_types_toggle[graph_types_toggle_index]);
          wattroff(win_toggle, A_BOLD);
          UpdateStats(win_feedback, scaled_amplitude, frequency);
        }
        break;

      case KEY_LEFT:
        if (graph_types_toggle_index == 0) {
          graph_types_toggle_index = 3;
        } else {
          graph_types_toggle_index--;
        }
        graph_type = graph_types_toggle_index;
        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
        WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
        wattron(win_toggle, A_BOLD);
        wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 16,
                  graph_types_toggle[graph_types_toggle_index]);
        wattroff(win_toggle, A_BOLD);
        break;
      case KEY_RIGHT:
        if (graph_types_toggle_index == 3) {
          graph_types_toggle_index = 0;
        } else {
          graph_types_toggle_index++;
        }
        graph_type = graph_types_toggle_index;
        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
        WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
        wattron(win_toggle, A_BOLD);
        wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 16,
                  graph_types_toggle[graph_types_toggle_index]);
        wattroff(win_toggle, A_BOLD);
        break;
      default:
        break;
    }
    wclear(win_wave_plot);
    WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
    DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
    PlotGraph(win_wave_plot, win_feedback, graph_type, amplitude,
              scaled_amplitude, frequency, phase_shift, win_wave_plot_height,
              win_wave_plot_width);

    wrefresh(stdscr);
    wrefresh(win_wave_plot);
    wrefresh(win_description);
    wrefresh(win_feedback);
    wrefresh(win_toggle);
    key = getch();
    phase_shift += 1.0;
    if (phase_shift / frequency >= (float)win_wave_plot_width) {
      phase_shift = 0.0;
    }
    usleep(time_period_ms * BASE_DELAY);
  }

  endwin(); /* End curses mode */
#endif
}

void DrawAxes(WINDOW* win, int win_wave_plot_height, int win_wave_plot_width) {
  // draw y axis
  int i;
  for (i = 2; i < win_wave_plot_height - 2; i++) {
    mvwprintw(win, i, 5, "|");
  }
  mvwprintw(win, 1, 5, "^");

  // draw x axis
  for (i = 2; i < win_wave_plot_width - 3; i++) {
    if (i == 5) {
      mvwprintw(win, win_wave_plot_height / 2, i, "0");
    } else {
      mvwprintw(win, win_wave_plot_height / 2, i, "-");
    }
  }
  mvwprintw(win, win_wave_plot_height / 2, win_wave_plot_width - 3, ">");
}

void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, GraphType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width) {
  int x, y;
  switch (type) {
    case SINE: {
      double i, ratio;
      for (i = 0.0; i < (win_wave_plot_width + phase_shift) - 3; i += 1.0) {
        if ((i - phase_shift) <= 5) {
          continue;
        }
        ratio = (2.0 * M_PI) / (win_wave_plot_width);
        x = (int)(i - phase_shift);
        y = (int)(scaled_amplitude * sin(i * ratio * frequency) +
                  win_wave_plot_height / 2.0);

        PlotPoint(win_wave_plot, x, y);
      }
      break;
    }
    case SQUARE: {
      // TODO
      break;
    }
    case TRIANGULAR: {
      // TODO
      break;
    }
    case SAWTOOTH: {
      // TODO
      break;
    }
    default:
      break;
  }

  UpdateStats(win_feedback, amplitude, frequency);
}

void PlotPoint(WINDOW* win, int x, int y) {
  wattron(win, A_BOLD);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win, y, x, ".");
  wattroff(win, A_BOLD);
  wattroff(win, COLOR_PAIR(1));
}

void UpdateStats(WINDOW* win, float amplitude, float frequency) {
  float period;
  wattron(win, A_BOLD);
  //   wattron(win, A_STANDOUT);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  period = 1 / frequency;
  mvwprintw(win, 2, 2, "Amplitude: ");
  //   wattron(win, COLOR_PAIR(3));
  mvwprintw(win, 3, 2, "Frequency: ");
  //   wattron(win, COLOR_PAIR(4));
  mvwprintw(win, 4, 2, "Period: ");

  //   wattroff(win, A_STANDOUT);
  //   wattron(win, COLOR_PAIR(6));
  mvwprintw(win, 2, 15, "%.2f", amplitude);
  mvwprintw(win, 3, 15, "%.2f Hz", frequency);
  mvwprintw(win, 4, 15, "%.2f s", period);
  wattroff(win, A_BOLD);
}

void WindowDesign(WINDOW* win_wave_plot, WINDOW* win_description,
                  WINDOW* win_feedback, WINDOW* win_toggle) {
  wattron(win_feedback, A_BOLD);
  wattron(win_feedback, COLOR_PAIR(3));
  wattron(win_wave_plot, A_BOLD);
  wattron(win_wave_plot, COLOR_PAIR(3));
  wattron(win_description, A_BOLD);
  wattron(win_description, COLOR_PAIR(3));
  wattron(win_toggle, A_BOLD);
  wattron(win_toggle, COLOR_PAIR(3));
  box(win_wave_plot, 0, 0);
  box(win_description, 0, 0);
  box(win_feedback, 0, 0);
  box(win_toggle, 0, 0);
  wattron(win_feedback, A_STANDOUT);
  wattron(win_wave_plot, A_STANDOUT);
  wattron(win_description, A_STANDOUT);
  wattron(win_toggle, A_STANDOUT);
  mvwprintw(win_wave_plot, 0, 2, " Wave Plot ");
  mvwprintw(win_description, 0, 2, " Description ");
  mvwprintw(win_feedback, 0, 2, " Statistics ");
  mvwprintw(win_toggle, 0, 2, " Controls ");
  wattroff(win_feedback, A_BOLD);
  wattroff(win_feedback, A_STANDOUT);
  wattroff(win_feedback, COLOR_PAIR(3));
  wattroff(win_wave_plot, A_BOLD);
  wattroff(win_wave_plot, A_STANDOUT);
  wattroff(win_wave_plot, COLOR_PAIR(3));
  wattroff(win_description, A_BOLD);
  wattroff(win_description, A_STANDOUT);
  wattroff(win_description, COLOR_PAIR(3));
  wattroff(win_toggle, A_BOLD);
  wattroff(win_toggle, A_STANDOUT);
  wattroff(win_toggle, COLOR_PAIR(3));
}