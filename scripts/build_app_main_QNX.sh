# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."

cd ../build
# Compilation
cc -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ -c ../app/app_main.c

# Linking
cc app_main.o terminal_ui.o -o ./bin/app_main -lm -lncurses 
echo "Compilation complete. Running executable... "
echo "======================================================="
./bin/app_main
