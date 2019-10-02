# Duino-hacks
Tiny Arduino (and related) hacks not worth of their own repository.

Posibly useful, but not too well documented.

## stdio_test
How to connect stdio (especially printf()) to an Arduino Serial port, using avr-libc's fdevopen() function.

## delay_without_delaying
Some function wrappers for easily implementing non-blocking delay code.  Works like:

~~~
if (delay_without_delaying(ledtime, 500)) {
    ledstate = !ledstate;
    digitalWrite(13, ledstate);
}
~~~

## ipchecksum_test
Improved ip checksum code (inline asm) for EtherCard and similar.

## test_trng
Demo sketch that initializes and uses the TRNG (True Random Number Generator) peripheral present on the SAMD51.

## fastdigitIO
A new method of implementing digitalWriteFast() and similar that does not need to be modified to work with different board pinouts or different AVR chips.  AVR, SAM, and SAMD versions are implemented.  The "trick" is reasonably well documented.

## systick_delay
A hack for implementing "delay for N clock cycles" that uses the sysTick timer present on all Cortex-M chips, in order to get near-exact results even in the face of things like "wait states", "flash accelerators", and cache memory.  The sysTick timer has to be running (it always is, in Arduino code), and the maximum cycle count used in the delay should be less than the systick reload value (on Arduino, less than 1ms worth of CPU cycles.)

## atmega4809
boards.txt and variants/pins_arduino.h for the Atmel/Microchip ATmega4809 Xplained Pro evaluation board.

## Serial9_test
experimental non-buffered routines for 9bit IO on AVR Arduinos.  Uses Serial.begin() for basic setup, and then "takes over" and provides read/write functions for 9bit characters.   Does not support Stream...
