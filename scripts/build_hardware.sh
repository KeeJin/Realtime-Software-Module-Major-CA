# Simple script to automate compilation and execution
echo "INFO: This script assumes that you have cloned the repository in your home directory."
echo "Compiling..."
cd ../build

cc -I ../include/ -c ../src/input.c
cc -I ../include/ -c ../src/PCI_init.c
cc -I ../include/ -c ../src/waveform.c

cc  -I ../include/ -lm input.o PCI_init.o waveform.o -o main ../app/main.c

./main
