#include "terminal_ui.h"


#if HARDWARE
#include "waveform.h"
#include "input.h"
#include "PCI_init.h"
#else

int wave_type = 0;
int prev_wave_type = 0;
int current_wave_type = 0;
float period = 50;
float prev_vert_offset = 0;
float current_vert_offset = 0;
int dio_switch = 0;
int switch2_value(int dio_switch) { return 1; }
int switch3_value(int dio_switch) { return 0; }
#endif


#include <math.h>
#include <unistd.h>
int beeper;

void* DisplayTUI(void* args) {
  WINDOW* win_wave_plot;
  WINDOW* win_description;
  WINDOW* win_feedback;
  WINDOW* win_toggle;
  int win_wave_plot_height, win_wave_plot_width;
  int cached_y_max, cached_x_max, y_max, x_max;
  int win_panel_height, win_panel_width;
  int x_padding, y_padding, key;
  int graph_types_toggle_index = wave_type;
  float scaled_amplitude;
  GraphType graph_type_local;
  float amplitude_local;
  float frequency_local;
  float phase_shift_local;
  float vertical_offset_local;
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

#ifndef DEBUG
  // Initialise windows
  win_wave_plot =
      newwin(win_wave_plot_height, win_wave_plot_width, y_padding, x_padding);

  win_description =
      newwin(win_panel_height, win_panel_width * 2, y_padding, x_padding);
  mvwin(win_description, y_padding + win_wave_plot_height, x_padding);

  win_feedback =
      newwin(win_panel_height * 2 / 3, win_panel_width, y_padding, x_padding);

  mvwin(win_feedback,
        y_padding + win_wave_plot_height + win_panel_height * 1 / 3,
        x_padding + win_panel_width * 2);

  win_toggle =
      newwin(win_panel_height * 1 / 3, win_panel_width, y_padding, x_padding);

  mvwin(win_toggle, y_padding + win_wave_plot_height,
        x_padding + 2 * win_panel_width);

  WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
  wattron(win_toggle, A_BOLD);
  wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win_toggle, 2, 2, "Graph Type: ");
  mvwprintw(win_toggle, 2, 16, graph_types_toggle[graph_types_toggle_index]);
  wattroff(win_toggle, A_BOLD);

  wattron(win_description, A_BOLD);
  wattron(win_description, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win_description, 2, 2, "Potentiometer 0 : Period (25 to 50 ms)");
  mvwprintw(win_description, 3, 2, "Potentiometer 1 : Amplitude (0 to 5 V)");
  mvwprintw(win_description, 5, 2, "Digital Switch 0: Stops the program");
  mvwprintw(win_description, 6, 2, "Digital Switch 1: Mute/Unmute beeping");
  mvwprintw(win_description, 7, 2, "Digital Switch 2: Saved values/Live");
  mvwprintw(win_description, 8, 2,
            "Digital Switch 3: TUI/Oscilloscope display");
  mvwprintw(win_description, 9, 2,
            "4 Digital LEDS  : Corresponds to switch state");
  mvwprintw(win_description, 11, 2, "L/R arrow keys : Wave selector");
  mvwprintw(win_description, 12, 2,
            "U/D arrow keys : Vertical offset (-5 to 5 V)");

  wattroff(win_description, A_BOLD);
  wrefresh(stdscr);
  wrefresh(win_description);

#endif
  // pthread_mutex_lock(&mutex);
  graph_type_local = wave_type;
  amplitude_local = amplitude;
  frequency_local = 1 / period * 1000;
  frequency = 1 / period * 1000;
  vertical_offset_local = vertical_offset;
  phase_shift_local = phase_shift;
  time_period_ms_local = time_period_ms;
  // pthread_mutex_unlock(&mutex);
  vertical_offset_local =
      vertical_offset_local /
      (0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0)) *
      ((float)win_wave_plot_height / 2.0);
  scaled_amplitude =
      0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0);
  DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
           vertical_offset_local);
  PlotGraph(win_wave_plot, win_feedback, graph_type, amplitude,
            scaled_amplitude, frequency, phase_shift, win_wave_plot_height,
            win_wave_plot_width);
#ifndef DEBUG
  // while (key != 'q') {
  while (1) {
    // pthread_mutex_lock(&mutex);
    graph_type_local = wave_type;
    amplitude_local = amplitude;
    frequency_local = 1 / period * 1000;
    frequency = 1 / period * 1000;
    vertical_offset_local = vertical_offset;
    phase_shift_local = phase_shift;
    time_period_ms_local = time_period_ms;
    //   pthread_mutex_unlock(&mutex);
    vertical_offset_local =
        vertical_offset_local /
        (0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0)) *
        ((float)win_wave_plot_height / 2.0);
    scaled_amplitude =
        0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0);
    if (switch2_value(dio_switch)) {
      if ((vertical_offset == prev_vert_offset) || (wave_type == prev_wave_type))
      // if (0)
      {
        vertical_offset = current_vert_offset;
        wave_type = current_wave_type;
        graph_type = wave_type;

        graph_types_toggle_index = wave_type;

        graph_type_local = wave_type; /*
         amplitude_local = amplitude;
         frequency_local = 1/period*1000;
             frequency = 1/period*1000;
         vertical_offset_local = vertical_offset;
         phase_shift_local = phase_shift;
         time_period_ms_local = time_period_ms;*/

        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                 vertical_offset_local);
        WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
        wattron(win_toggle, A_BOLD);
        wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 16,
                  graph_types_toggle[graph_types_toggle_index]);
        wattroff(win_toggle, A_BOLD);
      }
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
            wresize(win_description, win_panel_height, win_panel_width * 2);
            wresize(win_feedback, win_panel_height * 2 / 3, win_panel_width);
            wresize(win_toggle, win_panel_height * 1 / 3, win_panel_width);

            mvwin(win_wave_plot, y_padding, x_padding);
            mvwin(win_description, y_padding + win_wave_plot_height, x_padding);
            mvwin(win_feedback,
                  y_padding + win_wave_plot_height + win_panel_height * 1 / 3,
                  x_padding + win_panel_width * 2);
            mvwin(win_toggle, y_padding + win_wave_plot_height,
                  x_padding + 2 * win_panel_width);

            wclear(win_wave_plot);
            wclear(win_description);
            wclear(win_feedback);
            wclear(win_toggle);

            DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                     vertical_offset_local);
            WindowDesign(win_wave_plot, win_description, win_feedback,
                         win_toggle);
            wattron(win_toggle, A_BOLD);
            wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
            mvwprintw(win_toggle, 2, 2, "Graph Type: ");
            mvwprintw(win_toggle, 2, 16,
                      graph_types_toggle[graph_types_toggle_index]);
            wattroff(win_toggle, A_BOLD);

            wattron(win_description, A_BOLD);
            wattron(win_description, COLOR_PAIR(MAIN_TEXT_COLOUR));
            mvwprintw(win_description, 2, 2,
                      "Potentiometer 0 : Period (25 to 50 ms)");
            mvwprintw(win_description, 3, 2,
                      "Potentiometer 1 : Amplitude (0 to 5 V)");
            mvwprintw(win_description, 5, 2,
                      "Digital Switch 0: Stops the program");
            mvwprintw(win_description, 6, 2,
                      "Digital Switch 1: Mute/Unmute beeping");
            mvwprintw(win_description, 7, 2,
                      "Digital Switch 2: Saved values/Live");
            mvwprintw(win_description, 8, 2,
                      "Digital Switch 3: TUI/Oscilloscope display");
            mvwprintw(win_description, 9, 2,
                      "4 Digital LEDS  : Corresponds to switch state");
            mvwprintw(win_description, 11, 2, "L/R arrow keys : Wave selector");
            mvwprintw(win_description, 12, 2,
                      "U/D arrow keys : Vertical offset (-5 to 5 V)");

            wattroff(win_description, A_BOLD);
            wrefresh(stdscr);
            wrefresh(win_description);

            UpdateStats(win_feedback, scaled_amplitude, frequency, vertical_offset);
          }
          break;
        case KEY_UP:
          vertical_offset += VERT_OFFSET_INCREMENT;
          if (vertical_offset >= UPPER_LIMIT_VOLTAGE) vertical_offset = UPPER_LIMIT_VOLTAGE;
          current_vert_offset = vertical_offset;

          break;
        case KEY_DOWN:
          vertical_offset -= VERT_OFFSET_INCREMENT;
          if (vertical_offset <= LOWER_LIMIT_VOLTAGE) vertical_offset = LOWER_LIMIT_VOLTAGE;
          current_vert_offset = vertical_offset;

          break;

        case KEY_LEFT:
          if (graph_types_toggle_index == 0) {
            graph_types_toggle_index = 3;
          } else {
            graph_types_toggle_index--;
          }
          graph_type = graph_types_toggle_index;
          wave_type = graph_types_toggle_index;
          current_wave_type = wave_type;
          wclear(win_wave_plot);
          wclear(win_toggle);
          DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                   vertical_offset_local);
          WindowDesign(win_wave_plot, win_description, win_feedback,
                       win_toggle);
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
          wave_type = graph_types_toggle_index;
          current_wave_type = wave_type;
          wclear(win_wave_plot);
          wclear(win_toggle);
          DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                   vertical_offset_local);
          WindowDesign(win_wave_plot, win_description, win_feedback,
                       win_toggle);
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
    } else {
      vertical_offset = prev_vert_offset;
      wave_type = prev_wave_type;
      graph_types_toggle_index = prev_wave_type;
      graph_type = graph_types_toggle_index;
      wclear(win_wave_plot);
      wclear(win_toggle);
      DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
               vertical_offset_local);
      WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
      wattron(win_toggle, A_BOLD);
      wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
      mvwprintw(win_toggle, 2, 2, "Graph Type: ");
      mvwprintw(win_toggle, 2, 16,
                graph_types_toggle[graph_types_toggle_index]);
      wattroff(win_toggle, A_BOLD);
    }
    wclear(win_wave_plot);
    WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
    DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
             vertical_offset_local);
    PlotGraph(win_wave_plot, win_feedback, graph_type, amplitude,
              scaled_amplitude, frequency, phase_shift, win_wave_plot_height,
              win_wave_plot_width);

    wrefresh(stdscr);
    wrefresh(win_wave_plot);
    
    wrefresh(win_feedback);
    wrefresh(win_toggle);
    key = getch();
    phase_shift += 1.0;
    if (phase_shift * period >= (float)win_wave_plot_width * 25) {
      phase_shift = 0.0;
      if (beeper && !switch3_value(dio_switch)) {
        putchar(7);
        printf("\n");
      }
    }
    usleep(time_period_ms * BASE_DELAY);
  }

  endwin(); /* End curses mode */
#endif
}

void DrawAxes(WINDOW* win, int win_wave_plot_height, int win_wave_plot_width,
              float vertical_offset) {
  // draw y axis
  int i;

  for (i = 2; i < win_wave_plot_height - 2; i++) {
    mvwprintw(win, i, 5, "|");
  }
  mvwprintw(win, 1, 5, "^");

  // draw x axis
  for (i = 2; i < win_wave_plot_width - 3; i++) {
    if (i == 5) {
      mvwprintw(win, win_wave_plot_height / 2 + (int)vertical_offset, i, "0");
    } else {
      mvwprintw(win, win_wave_plot_height / 2 + (int)vertical_offset, i, "-");
    }
  }
  mvwprintw(win, win_wave_plot_height / 2 + vertical_offset,
            win_wave_plot_width - 3, ">");
}

void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, GraphType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width) {
  int x, y, repeat;
  double i, ratio;
  frequency /= 10;

  if (switch3_value(dio_switch)) {
  } else {
    switch (type) {
      case SINE: {
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
        for (repeat = 0; repeat < 8 * frequency; repeat++)  // number of cycles
        {
          for (i = win_wave_plot_width * (2 * repeat) / (2 * frequency);
               i < win_wave_plot_width * (2 * repeat + 1) / (2 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            // Find Y
            x = i - phase_shift;
            y = scaled_amplitude;
            y += win_wave_plot_height / 2;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          for (i = win_wave_plot_width * (2 * repeat + 1) / (2 * frequency);
               i < win_wave_plot_width * (2 * repeat + 2) / (2 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            // Find Y
            x = i - phase_shift;
            y = -scaled_amplitude;
            y += win_wave_plot_height / 2;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          x = win_wave_plot_width * (2 * repeat + 1) / (2 * frequency) -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          x = win_wave_plot_width * (2 * repeat + 2) / (2 * frequency) -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }
        }
        break;
      }
      case TRIANGULAR: {
        for (repeat = 0; repeat < 8 * frequency; repeat++)  // number of cycles
        {
          for (i = win_wave_plot_width * (2 * repeat) / (2 * frequency);
               i < win_wave_plot_width * (2 * repeat + 1) / (2 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            x = i - phase_shift;
            y = scaled_amplitude -
                scaled_amplitude / win_wave_plot_width * 4 * i * frequency;
            y += win_wave_plot_height / 2;
            y += (scaled_amplitude * repeat) * 4;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          for (i = win_wave_plot_width * (2 * repeat + 1) / (2 * frequency);
               i < win_wave_plot_width * (2 * repeat + 2) / (2 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            x = i - phase_shift;
            y = -3 * scaled_amplitude +
                scaled_amplitude / win_wave_plot_width * 4 * i * frequency;
            y += win_wave_plot_height / 2;
            y -= (scaled_amplitude * repeat) * 4;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }
        }
        break;
      }
      case SAWTOOTH: {
        for (repeat = 0; repeat < 8 * frequency; repeat++)  // number of cycles
        {
          for (i = win_wave_plot_width * (2 * repeat) / (1 * frequency);
               i < win_wave_plot_width * (2 * repeat + 1) / (1 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            x = i - phase_shift;
            y = scaled_amplitude -
                scaled_amplitude / win_wave_plot_width * 2 * i * frequency;
            y += win_wave_plot_height / 2;
            y += (scaled_amplitude * repeat) * 4;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }
          for (i = win_wave_plot_width * (2 * repeat + 1) / (1 * frequency);
               i < win_wave_plot_width * (2 * repeat + 2) / (1 * frequency);
               i += 1.0) {
            if ((i - phase_shift) <= 5) {
              continue;
            }
            x = i - phase_shift;
            y = 3 * scaled_amplitude -
                scaled_amplitude / win_wave_plot_width * 2 * i * frequency;
            y += win_wave_plot_height / 2;
            y += (scaled_amplitude * repeat) * 4;

            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          x = win_wave_plot_width * (2 * repeat + 1) / (1 * frequency) -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          x = win_wave_plot_width * (2 * repeat + 2) / (1 * frequency) -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }
        }

        break;
      }
      default:
        break;
    }
  }

  UpdateStats(win_feedback, amplitude, frequency, vertical_offset);
}

void PlotPoint(WINDOW* win, int x, int y) {
  wattron(win, A_BOLD);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win, y, x, ".");
  wattroff(win, A_BOLD);
  wattroff(win, COLOR_PAIR(1));
}

void UpdateStats(WINDOW* win, float amplitude, float frequency,
                 float vert_offset) {
  // float period;
  wattron(win, A_BOLD);
  //   wattron(win, A_STANDOUT);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  // period = 1 / frequency;
  mvwprintw(win, 2, 2, "Amplitude       :        V");
  //   wattron(win, COLOR_PAIR(3));
  mvwprintw(win, 3, 2, "Frequency       :        Hz");
  //   wattron(win, COLOR_PAIR(4));
  mvwprintw(win, 4, 2, "Period          :        ms");
  mvwprintw(win, 5, 2, "Vertical Offset :        V");

  //   wattroff(win, A_STANDOUT);
  //   wattron(win, COLOR_PAIR(6));
  mvwprintw(win, 2, 20, "%.2f ", amplitude);
  mvwprintw(win, 3, 20, "%.2f ", frequency);
  mvwprintw(win, 4, 20, "%.2f ", period);
  mvwprintw(win, 5, 20, "%.2f ", vertical_offset);
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