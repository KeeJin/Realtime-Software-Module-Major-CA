# Realtime-Software-Module-Major-CA

[Running full code, with hardware]
1. Go to path /include
2. In file PCI_init.h (line 15), choose PCI board type.
#define PCIe		0		// 0: PCI 1: PCIe 

3. In file terminal_ui.h (line 10), set hardware = 1
#define HARDWARE 	1 		// 0: without hardware; 1: with hardware 
4. Go to path /scripts
5. [IN QNX] 
	>>sh build_hardware.sh
6. [To run with command arguments] 
	>>sh build_hard.sh
	>>cd ../build
	>>./main t:0 v:0 
	
	
[Running without hardware (only TUI)]
1. Go to path /include
2. In file terminal_ui.h (line 10), set hardware = 0
#define HARDWARE 	0 		// 0: without hardware; 1: with hardware 
3. Go to path /scripts
4. [in QNX] 
	>>sh build_app_main_QNX.sh
5. [in UBUNTU] 
	>>bash build_app_main.sh
	
[Running without hardware (only TUI) on WINDOWS] --> UNTESTED 
1. Go to path /include
2. In file terminal_ui.h (line 10), set hardware = 0
#define HARDWARE 	0 		// 0: without hardware; 1: with hardware 
3. In file terminal_ui.h (line 4), change ncurses library
#include <ncursesw/ncurses.h>
4. Go to path /scripts
5. [in WINDOWS] 
	>>bash build_app_main.sh
