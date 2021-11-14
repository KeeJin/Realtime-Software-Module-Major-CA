#include <stdio.h>
//#include <ncursesw/ncurses.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define DEFAULT_AMPLITUDE 0.75
#define DEFAULT_PERIOD 1
#define DEFAULT_DELAY_MULTIPLIER 100 //convert micro to milli
#define BASE_DELAY 250               //time in ms
double ratio;
double x;
double y;
double repeat;
unsigned char delayx;
short phase_shift;
double amplitude;
double period;
int wave_type = 3;

/*
int N = 50;

typedef unsigned short ushort;

ushort printwave(double amplitude, double period,    //draws one frame of the entire wave
    short phase_shift) {

  for (x=0.0; x < (N+phase_shift) ; x += 1.0) {

   

    // Find Y 
    ratio = (2.0*M_PI)/N;
    y = sin(period*x*ratio);
    y *= amplitude; 
    y += N/2;
    

    // Print cell
    mvprintw((int)(y), (int)(x-phase_shift), " ");

  }

  

}
*/
typedef unsigned short ushort;

ushort printwave(double amplitude, double period, //draws one frame of the entire wave
                 short phase_shift)
{

  if (wave_type == 0) //sine
  {

    for (x = 0.0; x < (COLS + phase_shift); x += 1.0)
    {
      // Find Y
      ratio = (2.0 * M_PI) / LINES;
      y = sin(period * x * ratio);
      y *= amplitude;
      y += LINES / 2;

      // Print cell
      mvprintw((int)(y), (int)(x - phase_shift), " ");
    }
  }
  if (wave_type == 1) //square
  {
    for (repeat = 0; repeat < 8 * period; repeat++) //number of cycles
    {
      for (x = COLS * (2 * repeat) / (8 * period); x < COLS * (2 * repeat + 1) / (8 * period); x += 1.0)
      {
        // Find Y
        y = amplitude;
        y += LINES / 2;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }
      for (x = COLS * (2 * repeat + 1) / (8 * period); x < COLS * (2 * repeat + 2) / (8 * period); x += 1.0)
      {
        // Find Y
        y = -amplitude;
        y += LINES / 2;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }
    }
  }
  //float temp1 = 16.41;
  //float temp2 = 4.11;
  

  if (wave_type == 2) //triangle
  {
    for (repeat = 0; repeat < 16 * period; repeat++) //number of cycles
    {
      for (x = COLS * (2 * repeat) / (8 * period); x < COLS * (2 * repeat + 1) / (8 * period); x += 1.0)
      {
        // Find Y

        y = amplitude - amplitude / COLS * 16.41 * x * period;
        y += LINES / 2;
        y += (amplitude * repeat) * 4.11;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }
      for (x = COLS * (2 * repeat + 1) / (8 * period); x < COLS * (2 * repeat + 2) / (8 * period); x += 1.0)
      {
        // Find Y

        y = -3 * amplitude + amplitude / COLS * 16.41 * x * period;
        y += LINES / 2;
        //y *= -1;
        y -= (amplitude * repeat) * 4.11;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }
    }
  }

  if (wave_type == 3) //sawtooth
  {
    for (repeat = 0; repeat < 16 * period; repeat++) //number of cycles
    {
      for (x = COLS * (2 * repeat) / (8 * period); x < COLS * (2 * repeat + 1) / (8 * period); x += 1.0)
      {
        // Find Y

        y = amplitude - amplitude / COLS * 16.41 * x * period;
        y += LINES / 2;
        y += (amplitude * repeat) * 4.11;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }

      x = COLS * (2 * repeat + 1) / (8 * period);
      for (y = LINES / 2 - amplitude; y < LINES / 2 + amplitude; y++)
        mvprintw((int)(y), (int)(x - phase_shift), " ");

      for (x = COLS * (2 * repeat + 1) / (8 * period); x < COLS * (2 * repeat + 2) / (8 * period); x += 1.0)
      {
        // Find Y

        y = 3 * amplitude - amplitude / COLS * 16.41 * x * period;;
        y += LINES / 2;
        y += (amplitude * repeat) * 4.11;

        // Print cell
        mvprintw((int)(y), (int)(x - phase_shift), " ");
      }

      x = COLS * (2 * repeat + 2) / (8 * period); 
      for (y = LINES / 2 - amplitude; y < LINES / 2 + amplitude; y++)
        mvprintw((int)(y), (int)(x - phase_shift), " ");
    }
  }
}
int main(int argc, char *argv[])
{

  // Curses init
  WINDOW *screen = initscr();
  start_color();
  nodelay(screen, 1);
  cbreak();
  noecho();
  curs_set(0);
  keypad(screen, TRUE);

  attron(A_BOLD);
  attron(A_STANDOUT);

  // Wave attributes
  delayx = DEFAULT_DELAY_MULTIPLIER;

  phase_shift = 0;
  amplitude = (LINES / 2) * DEFAULT_AMPLITUDE;
  period = DEFAULT_PERIOD;

  while (1)
  {

    erase(); //erases for new frame

    printwave(amplitude, period, phase_shift); //draws a frame of the entire wave
    attron(COLOR_WHITE);
    mvprintw(0, 0, "PERIOD %.2f", period);
    mvprintw(1, 0, "AMP: %.2f", amplitude);
    mvprintw(2, 0, "PHASE: %d", phase_shift);
    mvprintw(3, 0, "DELAY: x%hu", delayx);

    refresh();

    switch (getch())
    {
    case KEY_UP:
    case 'k':
      wave_type = 1;
      amplitude += 1.0;
      break;
    case KEY_DOWN:
    case 'j':
      wave_type = 0;
      amplitude -= 1.0;
      break;
    case KEY_LEFT:
    case 'h':
      wave_type = 2;
      period -= .05;
      break;
    case KEY_RIGHT:
    case 'l':
      wave_type = 3;
      period += .05;
      break;
    case ' ':
      phase_shift = 0.0;
      break;
    case '+':
      delayx += 1;
      break;
    case '-':
      delayx -= 1;
      break;
    case 'q':
      endwin();
      return 0;
    default:
      break;
    }

    usleep(BASE_DELAY * DEFAULT_DELAY_MULTIPLIER);

    phase_shift += 1;

    //if (phase_shift * period / 0.94 >= LINES)
    //if (phase_shift * period * LINES * COLS / 10261 >= LINES)
    if (phase_shift * period >= LINES)
      phase_shift = 0;
  }

  endwin();
  return 0;
}