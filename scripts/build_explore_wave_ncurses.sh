# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
cd ../build


cc  -I ../include/ -o explore_wave_ncurses ../src/explore_wave_ncurses.c -lm -lncurses

./explore_wave_ncurses
