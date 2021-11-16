# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
src_dir=$(dirname "${BASH_SOURCE[${#BASH_SOURCE[@]} - 1]}")

# Check if a directory does not exist
if [ ! -d "${src_dir}/../build/bin" ] 
then
    echo "Creating build folder..." 
    mkdir ${src_dir}/../build/bin
fi

cd ${src_dir}/../build

# Compilation
cc -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ -c ../app/app_main.c

# Linking
cc app_main.o terminal_ui.o -o ./bin/app_main -lm -lncurses -lpthread
echo "Compilation complete. Running executable... "
echo "======================================================="
./bin/app_main
