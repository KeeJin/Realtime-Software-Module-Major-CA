Completed:
[Hardware]
- End code using digital switch 1
- Onoff sound using digital switch 2
- Amplitude + Period using 2 potentiometers 
- Arrow keys (updown) for vertical offset (horizontal offset effect cannot be seen because the wave is always moving)
- Waveforms Sine, Square, Triangular, Sawtooth
- Waveform parameters on terminal
- Metronome sound (ticks at the highest & lowest point of all waves, except sawtooth)

[Ncurses]
- Overall TUI layout
- Wave selector (using LR arrow keys)
- Animated wave (sine only for now)



Things to work on:
[Hardware]
- Make use of the 2 other switches
- Arrow keys (leftright) can be used to adjust duty cycle for square wave -- Once ncurses touch input works
- Autosaveload function

[Ncurses]
- Integrate animated sine wave into main tui
- Animate waves for other waveforms 
- Using mouse clicks as inputs 

[Ncurses + hardware integration]
- Using wave selector from ncurses thread (with LR arrow keys for now)
- Send wave params from input thread to ncurses thread
- Send wave from waveform thread to ncurses thread
