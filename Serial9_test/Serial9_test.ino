/*
 *  Example using WestfW Serial9 unbuffered 9bit UART support.
 *  Released to the public domain
 */

// get definitions and set up a 9bit serial interface "s9"
#include "Serial9.h"
Serial9 s9(&Serial);

uint16_t string16[] = {'1','6', ' ', 'b'+0x100, 'i'+0x100, 't'+0x100, ' ', 's', 't', 'r', 'i', 'n', 'g', '\n' }; 
void setup() {
  // Initialize the Serial Interface - Baud rate, etc, using the
  //  standard Serial functions.
  Serial.begin(9600);

  // Switch the interface to 9bit.  (hereafter, it needs to be access
  //  using the s9.xxx calls.
  s9.begin();
  // We can write normal strings with 9th bit 0
  s9.write("\n8bit banner banner\n");
  // we can write 16bit strings if we know the length
  s9.write(string16, sizeof(string16)/2);
  
}

char buffer[40];
void loop() {
  int c;
  if (s9.available() > 0) {
    c = s9.read();
    sprintf(buffer, "read character %x\n", c);
    s9.write(buffer);
    s9.write(c);
  }
}
