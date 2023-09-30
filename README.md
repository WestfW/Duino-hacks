# Duino-hacks
Tiny Arduino (and related) hacks not worth of their own repository.

Posibly useful, but not too well documented.

----------------------------------------------------

## ArduinoEEPROM
A sketch that implements stk500v1 for self-updating EEPROM of an ATmega chip.  Based on ArduinoISP, but writes to the chip's own EEPROM instead of an external SPI-connected device.

## BoardListManager
(not ready) Python code for generating "boardname.hide" entries to help pare down the "boards" menu when you have lots of "platforms", but not that many boards from each platform.

## DumbTXSWS
An Arduino library that does "Software Serial" Transmit-only using Arduino core functions in a trivial way.  This requires that digitalWrite() and delayMicroseconds() be "accurate" compared to the desired bitrate of transmission, which is likely to be the case on many newer zippy ARM targets (Adafruit M4 boards, for example.)
Tested on an Adafruit Metro M4 at up to 57600bps.  Seems to work.

## Serial9_test
experimental non-buffered routines for 9bit IO on AVR Arduinos.  Uses Serial.begin() for basic setup, and then "takes over" and provides read/write functions for 9bit characters.   Does not support Stream...

## atmega4809
boards.txt and variants/pins_arduino.h for the Atmel/Microchip ATmega4809 Xplained Pro evaluation board.

## delay_without_delaying
Some function wrappers for easily implementing non-blocking delay code.  Works like:
~~~
if (delay_without_delaying(ledtime, 500)) {
    ledstate = !ledstate;
    digitalWrite(13, ledstate);
}
~~~

## fastdigitIO
A new method of implementing digitalWriteFast() and similar that does not need to be modified to work with different board pinouts or different AVR chips.  AVR, SAM, and SAMD versions are implemented.  The "trick" is reasonably well documented.

## ipchecksum_test
Improved ip checksum code (inline asm) for EtherCard and similar.

## stdio_test
How to connect stdio (especially printf()) to an Arduino Serial port, using avr-libc's fdevopen() function.

## systick_delay
A hack for implementing "delay for N clock cycles" that uses the sysTick timer present on all Cortex-M chips, in order to get near-exact results even in the face of things like "wait states", "flash accelerators", and cache memory.  The sysTick timer has to be running (it always is, in Arduino code), and the maximum cycle count used in the delay should be less than the systick reload value (on Arduino, less than 1ms worth of CPU cycles.)


## test_sercomlib
Implments some macros that make it easier to initialize extra SERCOM peripherals as Serial ports on a SAMD51.

## test_trng
Demo sketch that initializes and uses the TRNG (True Random Number Generator) peripheral present on the SAMD51.


## SAMD_Explorer
Allows the examination of the internal state of a SAMD chip.  In theory, this should be a library; it has a non-blocking CLI interface that could be called from a normal user sketch.  Not yet, though.

## ARM_Explorer
Allows the examination of the internal state of an ARM chip.  Like SAMD_Explorer, but mostly focused on generic ARM functionality for CM4 (currently Renesas R4A as used on "Uno Minima R4"

## SingleNeo
Function and demo for driving the single neopixel present on some newer boards, using samd21 or rp2040

 
