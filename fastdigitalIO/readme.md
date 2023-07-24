# Fast Digital IO for Arduino

## Theory
### Why do we want digitalWriteFast() and digitalReadFast?
### Why are the Arduino funnction so slow?
### What are the limits of speed?

## Limitations - what do we lose by using digitalWriteFast()/etc?
- No turning off PWM.
- No turning on pullups.
- May only work with constant pin numbers
- May work better with constant pin values

## Other pieces of general strategy
### Constants
- the compiler includes the ability to detect when a value is a compile-time constant.
### Compiler Optimizations, "inline"
- Even array lookups (with  constant indices) can optimize to loading a constant.

### Fast Buses or address spaces
### Bit-processing peripherals

## Architecture Specific comments
### AVR
- SBI/CBI instructions
### ARM
- lacks special IO instructions; everything is a memory access.
- Peripheral buses can take several cycles to access.
### ARM Cortex M0
- IOBus - fast single-cycle access
### ARM Cortex M3/M4
