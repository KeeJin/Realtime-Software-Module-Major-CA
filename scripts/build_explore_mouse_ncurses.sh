# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
cd ../build


cc  -I ../include/ -o explore_mouse_ncurses ../src/explore_mouse_ncurses.c -lm -lncurses

./explore_mouse_ncurses
