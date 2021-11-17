# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
cd ../build
HARDWARE_CHECK="-DHARDWARE"
echo $1
if [ $HARDWARE_CHECK == $1 ]
then
echo "Compiling with Hardware."
cc $1 -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ -c ../src/input.c
cc -I ../include/ -c ../src/PCI_init.c
cc -I ../include/ -c ../src/waveform.c
cc $1 -I ../include/ input.o PCI_init.o waveform.o terminal_ui.o -o main ../app/main.c -lncurses -lm
else
echo "Compiling without Hardware."
cc -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ terminal_ui.o -o main ../app/main.c -lncurses -lm -lpthread
fi
./main


