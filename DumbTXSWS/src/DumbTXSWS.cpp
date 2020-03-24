/*
 * DumbTXSWS.cpp
 *

   Really Dumb Transmit-only Software Serial Implementation.
   Copyright 2020 By Bill Westfield (WestfW@yahoo.com)

This is aimed mainly at the new crop of fast ARM-based Arduinos that
don't implement SoftwareSerial, because they have "many" hardware serial
interfaces, but nevertheless might have cause to send data on some other pin.

This is free Open Source Software, distributed under the terms of the
well known "MIT License":

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

// 
// Includes
// 
#include <Arduino.h>
#include <DumbTXSWS.h>


//
// Constructor
//
DumbTXSWS::DumbTXSWS(uint8_t transmitPin, bool inverse_logic /* = false */)
{
    _txpin = transmitPin;
    _invert = inverse_logic;		/* xor mask to invert bits, maybe */
}

//
// Destructor
//
DumbTXSWS::~DumbTXSWS()
{
}

/* 
 * Public methods
 */

void DumbTXSWS::begin(long speed)
{
    _bitTime = 1000000 / speed;  /* bit time in microseconds */
    pinMode(_txpin, OUTPUT);
    digitalWrite(_txpin, HIGH ^ _invert); /* Idle state */
}

void DumbTXSWS::end()
{
}

/* 
 * Public methods standard for a stream device
 */

/*
 * Receive methods are not implemented.
 */
int DumbTXSWS::read() { return -1; }

int DumbTXSWS::available() { return 0; }

int DumbTXSWS::peek() { return -1; }


/*
 * Transmit methods
 */
size_t DumbTXSWS::write(uint8_t b)
{
    noInterrupts();
    digitalWrite(_txpin, LOW ^ _invert); /* start bit */
    delayMicroseconds(_bitTime);
    for (int i=0; i < 8; i++) {
	digitalWrite(_txpin, (b & 1) ^ _invert); /* Write data bit */
	delayMicroseconds(_bitTime);
	b = b >> 1;			/* Next data bit */
    }
    digitalWrite(_txpin, HIGH ^ _invert); /* Stop bits */
    delayMicroseconds(_bitTime * 3);	  /*  (3, just in case) */
    /* pin is now in idle state again. */
    interrupts();
}

void DumbTXSWS::flush()
{
    /* There is no tx buffering, simply return */
}
