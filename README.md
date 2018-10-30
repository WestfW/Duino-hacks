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
