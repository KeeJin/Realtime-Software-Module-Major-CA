# Simple script to automate compilation and execution
echo "Compiling..."

CURR_PATH="`dirname \"$0\"`"
mkdir -p $CURR_PATH/../build/bin
cd $CURR_PATH/../build
HARDWARE_CHECK="-DHARDWARE"
if [ $HARDWARE_CHECK == $1 ]
then
echo "Compiling with Hardware."
cc $1 -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ -c ../src/input.c
cc -I ../include/ -c ../src/PCI_init.c
cc -I ../include/ -c ../src/waveform.c
cc $1 -I ../include/ input.o PCI_init.o waveform.o terminal_ui.o -o bin/main ../app/main.c -lncurses -lm
./bin/main $2 $3
else
echo "Compiling without Hardware."
cc -I ../include/ -c ../src/terminal_ui.c
cc -I ../include/ terminal_ui.o -o bin/main ../app/main.c -lncurses -lm $1
./bin/main
fi



