# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
cd ../build
cc -o app_tui ../app/app_tui.c -lncurses -lm
echo "Compilation complete. Running executable... "
echo "======================================================="
./app_tui