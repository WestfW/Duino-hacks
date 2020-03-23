/*
DumbTXSWS.cpp
 Really Dumb Transmit-only Software Serial Implementation.
 By Bill Westfield (WestfW)

This is aimed mainly at the new crop of fast ARM-based Arduinos that
don't implement SoftwareSerial, because they have "many" hardware serial
interfaces, but nevertheless might have cause to send data on some other pin.

This is free Open Source Software, distributed under the terms of the
well known "MIT License."

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
    _invert = !inverse_logic;		/* xor mask to invert bits, maybe */
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
    _bitTime = 1000000 / speed;  // bit time in microseconds
    pinMode(_txpin, OUTPUT);
    digitalWrite(_txpin, _invert ? LOW : HIGH);
}

void DumbTXSWS::end()
{
}

/* 
 * Public methods standard for a stream device
 */

// Read data from buffer
int DumbTXSWS::read()
{
    return -1; // not implemented
}

int DumbTXSWS::available()
{
    return 0;
}

size_t DumbTXSWS::write(uint8_t b)
{
    noInterrupts();
    digitalWrite(_txpin, 0); // start bit
    delayMicroseconds(_bitTime);
    for (int i=0; i < 8; i++) {
	digitalWrite(_txpin, b & 1);	/* Write data bit */
	delayMicroseconds(_bitTime);
	b = b >> 1;			/* Next data bit */
    }
    digitalWrite(_txpin, 1);		/* Stop bits (3, just in case) */
    delayMicroseconds(_bitTime * 3);
    interrupts();
}

void DumbTXSWS::flush()
{
  // There is no tx buffering, simply return
}

int DumbTXSWS::peek()
{
    return -1;
}
