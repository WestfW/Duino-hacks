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
- the role of Link Time Optimization

### Fast Buses or address spaces
### Bit-processing peripherals

## Architecture Specific comments
### AVR
- SBI/CBI instructions
- But what about PGMSPACE?
- Classic Mega vs Mega0/xTiny/AVR-xx
- VPORT
### ARM
- lacks special IO instructions; everything is a memory access.
- Peripheral buses can take several cycles to access.
- Barrel shifter
- Port size
### ARM Cortex M0
- IOBus - fast single-cycle access
### ARM Cortex M3/M4
## Comments on Testing
It turns out that a loop containing digitalWriteFast() calls to toggle a pin is not a very good test of the speed of the function itself.
Once you use inline functions (or link-time optimization), the compiler is free to optimize the code in ways that obscure what al is being done.
For example, on most ARMs, all the loading of addresses and constants into registers will likely be moved outside of the loop itself.
That leaves you with a very fast loop containing only a couple of store instructions.  A nice fast waveform, but a poor measure of how well
deriving and loading the necessary constants is being done.

