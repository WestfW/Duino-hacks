Minimalized code for testing PWM bug

pre-processing is bypassed.

core contains only those source files necessary to build the example buggy program.

build scripts for:

arduino - uses the installed arduino code, and the sketch.ino.cpp file
standalone - uses the stripped core, including some empty .h files and mega's pins_arduino.h
nolto - like standalone, but with the lto switches removed
old - like standalone, but using the old compilers

The scripts will need to be edited with the location of your compiler/arduino install.

The "arduino" and "standalone" builds have been confirmed to produce the same .hex file as a build from the 1.8.10 IDE itself.
