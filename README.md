# Realtime-Software-Module-Major-CA

## Pre-requisites
- ncurses on C

## How to compile

#### 1. Go to path /include

#### 2. In file PCI_init.h (line 15), choose PCI board type.   
    
#### ``` #define PCIe		0		// 0: PCI 1: PCIe ```

#### 3. Run automated script:

- ### QNX 
	- Without hardware    
	``` sh build_main.sh ```    
	- With hardware    
	``` sh build_main.sh -DHARDWARE ```
	- With hardware + command line arguments    
	``` sh build_main.sh -DHARDWARE t:1 v:3 d:75```
   
 
- ### Ubuntu, without hardware
	``` ./build_main.sh -lpthread ```