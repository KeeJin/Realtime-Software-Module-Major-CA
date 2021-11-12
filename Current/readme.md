### Compiling the Program
```
cc -c inputs.c
cc -c PCI_init.c
cc -c waveform.c
cc -lm inputs.o PCI_init.o waveform.o -o main main.c
```
### Running the Program
```
./main t:wave_type v:vertical_offset
./main --> will run default values
```


search "FLAGGED!" in codes for things to be changed