# RCU simulator for STBs

This tool is used to inject keys to STBs, it reads routine files and repeats them in a loop. 
New routines can be created and passed as first argument to the executable. 

## Routine files
Each instruction consists of a "key time" pair, comments start with # and empty lines will be skipped, if an instruction cannot be interpreted a warning message will be printed and the instruction will be skipped.

The list of keys can be found in the include/keyboard.h file.

## Device
RcuSimulator tries to detect the correct device to open but sometimes it may fail and the keys are not injected, in such a case you can specify the device as the third parameter.

### Example of use
- RcuSimulator routine
- RcuSimulator routine device
  
- RcuSimulator NetflixNavigationAndPlay
- RcuSimulator NetflixNavigationAndPlay /dev/input/event

### Routine files
Example routines can be found in routines folder
