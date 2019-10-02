/*
  Serial9.cpp
  Support for unbuffered 8bit mode on Serial interfaces.
  Bill Westfield (WestfW) 2019
  Released to the public domain.

  General strategy:
  Let the user initialize the UART with Serial.begin(), and then use
  this library to "disconnect" the UART from the normal Arduino function,
  reset it to 9-bit mode (keeping any other settings), and then access the
  RX and TX registers directly (no interrupts, no buffering beyond what is
  provided by the hardware (2 bytes.)

  Because "reasons", this is not a "stream" subclass, so the .print() methods
  are not available.  Just various varieties of .write()

    write(uint16_t byte)         // Write a 9bit byte.
    write(const char *str)       // quoted strings are written with bit9 = 0
    write(const uint8_t *buf, size_t size);  // 8bit arrays also wrirten with bit9=0
    write(const char *buffer, size_t size)
    write(const uint16_t *buffer, size_t size); // 16bit arrays write 9 bits from each word
*/

#include <Arduino.h>
#include "Serial9.h"

Serial9::Serial9(HardwareSerial *hws)
{
  _hw = hws;  // save which UART we're using.
}

Serial9::~Serial9(){ }

// Begin: enable 9bit mode, turn of the regular serial.  Pick our registers
// based on the SerialN constructor (too bad we can't acually get the register
// FROM the SerialN (they're "private"), but this probably only works on a
// subset of AVRs, anyway.  (not on ATmega8, for instance.)
void Serial9::begin(void)
{
#if defined (HAVE_HWSERIAL0)
  if (_hw == &Serial) {
    _ucsra = &UCSR0A;
    _ucsrb = &UCSR0B;
    _ucsrc = &UCSR0C;
    _udr = &UDR0;
  }
#endif
#if defined (HAVE_HWSERIAL1)
  if (_hw == &Serial1) {
    _ucsra = &UCSR1A;
    _ucsrb = &UCSR1B;
    _ucsrc = &UCSR1C;
    _udr = &UDR1;
  }
#endif
#if defined (HAVE_HWSERIAL2)
  if (_hw == &Serial2) {
    _ucsra = &UCSR2A;
    _ucsrb = &UCSR2B;
    _ucsrc = &UCSR2C;
    _udr = &UDR2;
  }
#endif
#if defined (HAVE_HWSERIAL3)
  if (_hw == &Serial3) {
    _ucsra = &UCSR3A;
    _ucsrb = &UCSR3B;
    _ucsrc = &UCSR3C;
    _udr = &UDR3;
  }
#endif
  // disable interrupts, set 9bit mode, enable rx/tx
  //  This completely overwrites any previous UCSRB bits
  *_ucsrb = (1 << UCSZ02) | (1 << TXEN0) | (1 << RXEN0) | // 9bits, enabled
            (0 << UDRIE0) | (0 << TXCIE0) | (0 << RXCIE0); // ints off.

  // Enable 9bit size in UCSRC.  Leave the other bits alone (Parity, etc)
  *_ucsrc |= (1 << UCSZ00) | (1 << UCSZ01);

}

// Read data from buffer
int Serial9::read()
{
  uint8_t status = *_ucsra;
  int16_t data;
  if (!(status & (1 << RXC0)))
    return -1;
  data = (uint16_t)(*_ucsrb & (1 << RXB80)) << (8 - RXB80);
  data |= *_udr;
  _errors |= status & ((1 << UPE0) | (1 << DOR0) | (1 << FE0));
  return data;
}

int Serial9::available()
{
  uint8_t status = *_ucsra;
  if (status & (1 << RXC0))
    return 1;
  return 0;   // at least one byte is available.
}

size_t Serial9::write(uint16_t b)
{
  while ((*_ucsra & (1 << UDRE0)) == 0)
    ; // spin, waiting for data buffer empty

  if (b && (1 << 8)) { // 9th bit
    *_ucsrb |= (1 << TXB80);
  } else {
    *_ucsrb &= ~(1 << TXB80);
  }
  *_udr = b & 0xFF; // The other 8 bits.
  return 1;
}

// Write a set of 8bit bytes (9thbit 0)
size_t Serial9::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
    if (write(*buffer++)) n++;
    else break;
  }
  return n;
}

size_t Serial9::write(const uint16_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
    if (write(*buffer++)) n++;
    else break;
  }
  return n;
}
