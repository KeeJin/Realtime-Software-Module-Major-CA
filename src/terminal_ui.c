#include "terminal_ui.h"

#ifdef HARDWARE
#include "input.h"
#include "PCI_init.h"

#else
#define PCIe 0
int switch0_value(unsigned int dio_switch) { return 1; }
int switch1_value(unsigned int dio_switch) { return 1; }
int switch2_value(unsigned int dio_switch) { return 1; }
int switch3_value(unsigned int dio_switch) { return 0; }
#endif

#include <math.h>
#include <unistd.h>

void DisplayTUI() {
  WINDOW* win_wave_plot;
  WINDOW* win_description;
  WINDOW* win_feedback;
  WINDOW* win_toggle;
  int win_wave_plot_height, win_wave_plot_width;
  int cached_y_max, cached_x_max, y_max, x_max;
  int win_panel_height, win_panel_width;
  int x_padding, y_padding, key;
  int wave_types_toggle_index = SINE;
  float scaled_amplitude, scaled_vertical_offset, phase_shift;
  unsigned int dio_switch_local;
  WaveType wave_type_local;
  float amplitude_local;
  float frequency_local;
  float vertical_offset_local;
  float period_local;
  int duty_cycle_local;
  int time_period_ms_local;
  const char* wave_types_toggle[4];
  int prev_live = 1;
  int prev_switch0;
  int x_win_threshold, right_panel_width;  // for window sizing optimisation

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

  wave_types_toggle[0] = "SINE";
  wave_types_toggle[1] = "SQUARE";
  wave_types_toggle[2] = "TRIANGULAR";
  wave_types_toggle[3] = "SAW TOOTH";

  getmaxyx(stdscr, cached_y_max, cached_x_max);
  x_padding = 1;
  y_padding = 1;
  x_win_threshold = 105;
  if (cached_x_max < x_win_threshold) {
    right_panel_width = 28;
  } else {
    right_panel_width = 55;
  }
  win_panel_height = 15;
  win_wave_plot_height = cached_y_max - win_panel_height - 1;
  win_wave_plot_width = cached_x_max - (2 * x_padding);
  win_panel_width = win_wave_plot_width - right_panel_width;
  phase_shift = 0.0;

  pthread_mutex_lock(&mutex_common);
  amplitude_local = amplitude;
  period_local = period;
  frequency_local = 1 / period * 1000;
  time_period_ms_local = time_period_ms;
  dio_switch_local = dio_switch;
  duty_cycle_local = duty_cycle;
  vertical_offset_local = vertical_offset;
  wave_type_local = wave_type;
  pthread_mutex_unlock(&mutex_common);

  wave_types_toggle_index = wave_type_local;

  // Initialise windows
  win_wave_plot =
      newwin(win_wave_plot_height, win_wave_plot_width, y_padding, x_padding);

  win_description =
      newwin(win_panel_height, win_panel_width, y_padding, x_padding);
  mvwin(win_description, y_padding + win_wave_plot_height, x_padding);

  win_feedback =
      newwin(win_panel_height * 2 / 3, right_panel_width, y_padding, x_padding);

  mvwin(win_feedback,
        y_padding + win_wave_plot_height + win_panel_height * 1 / 3,
        x_padding + win_panel_width);

  win_toggle = newwin(win_panel_height * 1 / 3, right_panel_width,
                      y_padding, x_padding);

  mvwin(win_toggle, y_padding + win_wave_plot_height,
        x_padding + win_panel_width);

  WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
  wattron(win_toggle, A_BOLD);
  wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win_toggle, 2, 2, "Graph Type: ");
  mvwprintw(win_toggle, 2, 16, wave_types_toggle[wave_types_toggle_index]);
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
  mvwprintw(win_description, 10, 2, "L/R arrow keys  : Wave selector");
  mvwprintw(win_description, 11, 2, "U/D arrow keys  : Y-offset (-5 to 5 V)");
  if (wave_type_local == SQUARE)
    mvwprintw(win_description, 12, 2,
              "+/- keypress    : Duty Cycle (0 to 100%%)");
  else
    mvwprintw(win_description, 12, 2,
              "                                         ");

  wattroff(win_description, A_BOLD);
  wrefresh(stdscr);
  wrefresh(win_description);

  scaled_vertical_offset =
      0.8 * ((float)win_wave_plot_height / 2.0) * (vertical_offset_local / 5.0);
  scaled_amplitude =
      0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0);
  DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
           scaled_vertical_offset);
  UpdateStats(win_feedback, amplitude_local, frequency_local,
              vertical_offset_local, duty_cycle_local, wave_type_local);
  if (!switch3_value(dio_switch_local)) {
    PlotGraph(win_wave_plot, win_feedback, wave_type_local, amplitude_local,
              scaled_amplitude, frequency_local, phase_shift,
              win_wave_plot_height, win_wave_plot_width, duty_cycle_local);
  } else {
    // Inform user that display is currently on oscilloscope
    DisplayInactivePlot(win_wave_plot, win_wave_plot_width,
                        win_wave_plot_height);
  }
  prev_switch0 = switch0_value(dio_switch_local);
  while (switch0_value(dio_switch_local) == prev_switch0) {
    pthread_mutex_lock(&mutex_common);
    amplitude_local = amplitude;
    frequency_local = 1.0 / period * 1000.0;
    period_local = period;
    time_period_ms_local = time_period_ms;
    dio_switch_local = dio_switch;
    duty_cycle = duty_cycle_local;
    vertical_offset = vertical_offset_local;
    wave_type = wave_type_local;
    pthread_mutex_unlock(&mutex_common);

    scaled_vertical_offset =
        vertical_offset_local /
        (0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0)) *
        ((float)win_wave_plot_height / 2.0);
    scaled_amplitude =
        0.8 * ((float)win_wave_plot_height / 2.0) * (amplitude_local / 5.0);

    wattron(win_description, A_BOLD);
    wattron(win_description, COLOR_PAIR(MAIN_TEXT_COLOUR));
    if (wave_type_local == SQUARE)
      mvwprintw(win_description, 12, 2,
                "+/- keypress    : Duty Cycle (0 to 100%%)");
    else
      mvwprintw(win_description, 12, 2,
                "                                         ");

    wattroff(win_description, A_BOLD);
    wrefresh(stdscr);
    wrefresh(win_description);

    if (switch2_value(dio_switch_local)) {
      if ((vertical_offset_local == prev_vert_offset) ||
          (wave_type_local == prev_wave_type) ||
          duty_cycle_local == prev_duty_cycle)
      {
        prev_live = 1;
        pthread_mutex_lock(&mutex_common);
        vertical_offset = current_vert_offset;
        wave_type = current_wave_type;
        duty_cycle = current_duty_cycle;
        pthread_mutex_unlock(&mutex_common);

        wave_types_toggle_index = current_wave_type;

        wave_type_local = current_wave_type;

        duty_cycle_local = current_duty_cycle;

        wclear(win_wave_plot);
        wclear(win_toggle);
        DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                 scaled_vertical_offset);
        WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
        wattron(win_toggle, A_BOLD);
        wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
        mvwprintw(win_toggle, 2, 2, "Graph Type: ");
        mvwprintw(win_toggle, 2, 16,
                  wave_types_toggle[wave_types_toggle_index]);
        wattroff(win_toggle, A_BOLD);
      }
      UpdateStats(win_feedback, amplitude_local, frequency_local,
                  vertical_offset_local, duty_cycle_local, wave_type_local);
      switch (key) {
        case KEY_RESIZE:
          getmaxyx(stdscr, y_max, x_max);

          // Check if window size has change - if yes, recalculate
          if (y_max != cached_y_max || x_max != cached_x_max) {
            cached_x_max = x_max;
            cached_y_max = y_max;
            if (cached_x_max < x_win_threshold) {
              right_panel_width = 28;
            } else {
              right_panel_width = 55;
            }
            win_panel_height = 14;
            win_wave_plot_height = cached_y_max - win_panel_height - 1;
            win_wave_plot_width = cached_x_max - (2 * x_padding);
            win_panel_width = win_wave_plot_width - right_panel_width;

            wresize(win_wave_plot, win_wave_plot_height, win_wave_plot_width);
            wresize(win_description, win_panel_height, win_panel_width);
            wresize(win_feedback, win_panel_height * 2 / 3, right_panel_width);
            wresize(win_toggle, win_panel_height * 1 / 3 + 1,
                    right_panel_width);

            mvwin(win_wave_plot, y_padding, x_padding);
            mvwin(win_description, y_padding + win_wave_plot_height, x_padding);
            mvwin(
                win_feedback,
                y_padding + win_wave_plot_height + win_panel_height * 1 / 3 + 1,
                x_padding + win_panel_width);
            mvwin(win_toggle, y_padding + win_wave_plot_height,
                  x_padding + win_panel_width);

            wclear(win_wave_plot);
            wclear(win_description);
            wclear(win_feedback);
            wclear(win_toggle);

            DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                     scaled_vertical_offset);
            WindowDesign(win_wave_plot, win_description, win_feedback,
                         win_toggle);
            wattron(win_toggle, A_BOLD);
            wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
            mvwprintw(win_toggle, 2, 2, "Graph Type: ");
            mvwprintw(win_toggle, 2, 16,
                      wave_types_toggle[wave_types_toggle_index]);
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
            mvwprintw(win_description, 10, 2,
                      "L/R arrow keys  : Wave selector");
            mvwprintw(win_description, 11, 2,
                      "U/D arrow keys  : Y-offset (-5 to 5 V)");
            if (wave_type_local == SQUARE)
              mvwprintw(win_description, 12, 2,
                        "+/- keypress    : Duty Cycle (0 to 100%%)");
            else
              mvwprintw(win_description, 12, 2,
                        "                                         ");

            wattroff(win_description, A_BOLD);
            wrefresh(stdscr);
            wrefresh(win_description);

            UpdateStats(win_feedback, amplitude_local, frequency_local,
                        vertical_offset_local, duty_cycle_local,
                        wave_type_local);
          }
          break;
        case 43:  // + key
          if (wave_type_local != SQUARE) break;
          duty_cycle_local += DUTY_CYCLE_INCREMENT;
          if (duty_cycle_local >= 100) duty_cycle_local = 100;
          current_duty_cycle = duty_cycle_local;
          break;

        case 45:  // - key
          if (wave_type_local != SQUARE) break;
          duty_cycle_local -= DUTY_CYCLE_INCREMENT;
          if (duty_cycle_local <= 0) duty_cycle_local = 0;
          current_duty_cycle = duty_cycle_local;
          break;

        case KEY_UP:
          vertical_offset_local += VERT_OFFSET_INCREMENT;
          if (vertical_offset_local >= UPPER_LIMIT_VOLTAGE)
            vertical_offset_local = UPPER_LIMIT_VOLTAGE;
          current_vert_offset = vertical_offset_local;
          break;

        case KEY_DOWN:
          vertical_offset_local -= VERT_OFFSET_INCREMENT;
          if (vertical_offset_local <= LOWER_LIMIT_VOLTAGE)
            vertical_offset_local = LOWER_LIMIT_VOLTAGE;
          current_vert_offset = vertical_offset_local;
          break;

        case KEY_LEFT:
          if (wave_types_toggle_index == 0) {
            wave_types_toggle_index = 3;
          } else {
            wave_types_toggle_index--;
          }
          wave_type_local = wave_types_toggle_index;

          current_wave_type = wave_types_toggle_index;
          wclear(win_wave_plot);
          wclear(win_toggle);
          DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                   scaled_vertical_offset);
          WindowDesign(win_wave_plot, win_description, win_feedback,
                       win_toggle);
          wattron(win_toggle, A_BOLD);
          wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
          mvwprintw(win_toggle, 2, 2, "Graph Type: ");
          mvwprintw(win_toggle, 2, 16,
                    wave_types_toggle[wave_types_toggle_index]);
          wattroff(win_toggle, A_BOLD);
          break;
        case KEY_RIGHT:
          if (wave_types_toggle_index == 3) {
            wave_types_toggle_index = 0;
          } else {
            wave_types_toggle_index++;
          }
          wave_type_local = wave_types_toggle_index;

          current_wave_type = wave_types_toggle_index;
          wclear(win_wave_plot);
          wclear(win_toggle);
          DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
                   scaled_vertical_offset);
          WindowDesign(win_wave_plot, win_description, win_feedback,
                       win_toggle);
          wattron(win_toggle, A_BOLD);
          wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
          mvwprintw(win_toggle, 2, 2, "Graph Type: ");
          mvwprintw(win_toggle, 2, 16,
                    wave_types_toggle[wave_types_toggle_index]);
          wattroff(win_toggle, A_BOLD);
          break;
        default:
          break;
      }
    } else {
      pthread_mutex_lock(&mutex_common);
      vertical_offset = prev_vert_offset;
      wave_type = prev_wave_type;
      duty_cycle = prev_duty_cycle;
      pthread_mutex_unlock(&mutex_common);

      wave_types_toggle_index = prev_wave_type;
      wave_type_local = prev_wave_type;
      duty_cycle_local = prev_duty_cycle;
      wclear(win_wave_plot);
      wclear(win_toggle);
      DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
               scaled_vertical_offset);
      WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
      wattron(win_toggle, A_BOLD);
      wattron(win_toggle, COLOR_PAIR(MAIN_TEXT_COLOUR));
      mvwprintw(win_toggle, 2, 2, "Graph Type: ");
      mvwprintw(win_toggle, 2, 16, wave_types_toggle[wave_types_toggle_index]);
      wattroff(win_toggle, A_BOLD);
      if (prev_live) {
        prev_live = 0;
        UpdateStats(win_feedback, amplitude_local, frequency_local,
                    vertical_offset_local, duty_cycle_local, wave_type_local);
      }
    }
    wclear(win_wave_plot);
    WindowDesign(win_wave_plot, win_description, win_feedback, win_toggle);
    DrawAxes(win_wave_plot, win_wave_plot_height, win_wave_plot_width,
             scaled_vertical_offset);
    if (!switch3_value(dio_switch_local)) {
      PlotGraph(win_wave_plot, win_feedback, wave_type_local, amplitude_local,
                scaled_amplitude, frequency_local, phase_shift,
                win_wave_plot_height, win_wave_plot_width, duty_cycle_local);
    } else {
      // Inform user that display is currently on oscilloscope
      DisplayInactivePlot(win_wave_plot, win_wave_plot_width,
                          win_wave_plot_height);
    }

    wrefresh(stdscr);
    wrefresh(win_wave_plot);

    wrefresh(win_feedback);
    wrefresh(win_toggle);
    key = getch();
    while (getch() != ERR) {}  // clear buffer
    phase_shift += 1.0;
    if (phase_shift * period_local >= (float)win_wave_plot_width * 25) {
      phase_shift = 0.0;
    }
    usleep(time_period_ms * BASE_DELAY);
  }

  endwin(); /* End curses mode */
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
  mvwprintw(win, win_wave_plot_height / 2 + (int)vertical_offset,
            win_wave_plot_width - 3, ">");
}

void PlotGraph(WINDOW* win_wave_plot, WINDOW* win_feedback, WaveType type,
               float amplitude, float scaled_amplitude, float frequency,
               float phase_shift, int win_wave_plot_height,
               int win_wave_plot_width, int duty_cycle) {
  int x, y, repeat;
  float temp1, temp2;
  double i, ratio;
  frequency /= 10;

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
        for (i = win_wave_plot_width / frequency * (100 * repeat) / 100;
             i < win_wave_plot_width / frequency *
                     (100 * repeat + (100 - duty_cycle)) / 100;
             i += 1.0) {
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
            continue;
          }
          // Find Y
          x = i - phase_shift;
          y = scaled_amplitude;
          y += win_wave_plot_height / 2;

          // Print cell
          PlotPoint(win_wave_plot, (int)x, (int)y);
        }
        for (i = win_wave_plot_width / frequency *
                 (100 * repeat + (100 - duty_cycle)) / 100;
             i < win_wave_plot_width / frequency * (100 * repeat + 100) / 100;
             i += 1.0) {
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
            continue;
          }
          // Find Y
          x = i - phase_shift;
          y = -scaled_amplitude;
          y += win_wave_plot_height / 2;

          // Print cell
          PlotPoint(win_wave_plot, (int)x, (int)y);
        }

        if (duty_cycle > 0 && duty_cycle < 100) {
          x = win_wave_plot_width / frequency *
                  (100 * repeat + (100 - duty_cycle)) / 100 -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5 || (x) >= win_wave_plot_width - 3) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }

          x = win_wave_plot_width / frequency * (100 * repeat + 100) / 100 -
              phase_shift;
          for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
               y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
            if ((x) <= 5 || (x) >= win_wave_plot_width - 3) {
              continue;
            }
            // Print cell
            PlotPoint(win_wave_plot, (int)x, (int)y);
          }
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
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
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
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
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
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
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
          if ((i - phase_shift) <= 5 ||
              (i - phase_shift) >= win_wave_plot_width - 3) {
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
          if ((x) <= 5 || (x) >= win_wave_plot_width - 3) {
            continue;
          }
          // Print cell
          PlotPoint(win_wave_plot, (int)x, (int)y);
        }

        x = win_wave_plot_width * (2 * repeat + 2) / (1 * frequency) -
            phase_shift;
        for (y = win_wave_plot_height / 2 - scaled_amplitude + 2;
             y < win_wave_plot_height / 2 + scaled_amplitude; y++) {
          if ((x) <= 5 || (x) >= win_wave_plot_width - 3) {
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

  UpdateStats(win_feedback, amplitude, frequency, vertical_offset, duty_cycle,
              type);
}

void PlotPoint(WINDOW* win, int x, int y) {
  wattron(win, A_BOLD);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  mvwprintw(win, y, x, ".");
  wattroff(win, A_BOLD);
  wattroff(win, COLOR_PAIR(1));
}

void UpdateStats(WINDOW* win, float amplitude, float frequency,
                 float vert_offset, int duty_cycle, WaveType wave_type) {
  // float period;
  float period;
  frequency /= 60;
  period = 1 / frequency;
  #if PCIe
  amplitude /= 4;
  vert_offset /= 4;
  #endif
  wattron(win, A_BOLD);
  //   wattron(win, A_STANDOUT);
  wattron(win, COLOR_PAIR(MAIN_TEXT_COLOUR));
  // period = 1 / frequency;
  mvwprintw(win, 2, 2, "Amplitude    :        V");
  //   wattron(win, COLOR_PAIR(3));
  mvwprintw(win, 3, 2, "Frequency    :        Hz");
  //   wattron(win, COLOR_PAIR(4));
  mvwprintw(win, 4, 2, "Period       :        s");
  mvwprintw(win, 5, 2, "Y-offset     :        V");
  if (wave_type == SQUARE)
    mvwprintw(win, 6, 2, "Duty Cycle   :        %%");
  else
    mvwprintw(win, 6, 2, "                        ");

  //   wattroff(win, A_STANDOUT);
  //   wattron(win, COLOR_PAIR(6));
  mvwprintw(win, 2, 17, "%.2f ", amplitude);
  mvwprintw(win, 3, 17, "%.2f ", frequency);
  mvwprintw(win, 4, 17, "%.2f ", period);
  mvwprintw(win, 5, 17, "%.2f ", vert_offset);

  if (wave_type == SQUARE)
    mvwprintw(win, 6, 17, "%d ", duty_cycle);
  else
    mvwprintw(win, 6, 2, "                        ");
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

void DisplayInactivePlot(WINDOW* win, int win_width, int win_height) {
  wattron(win, A_BLINK);
  wattron(win, A_BOLD);
  wattron(win, COLOR_PAIR(1));
  mvwprintw(win, win_height / 2 - 3, win_width / 2 - 11,
            "Wave Display Inactive.");
  mvwprintw(win, win_height / 2 - 2, win_width / 2 - 15,
            "Please refer to oscilloscope,");
  mvwprintw(win, win_height / 2 - 1, win_width / 2 - 10, "or toggle switch 3!");
  wattroff(win, A_BLINK);
  wattroff(win, A_BOLD);
  wattroff(win, COLOR_PAIR(1));
}