/*==============================================================================

  ATmega4809-hosted High Voltage Parallel Programmer for ATtiny28
  by Bill Westfield, 2020.

  Based somewhat on (BSD Licensed) code from:
  [https://github.com/dilshan/avr-hv2]
  Copyright (c) 2020 Dilshan R Jayakody [jayakody2000lk at gmail dot com]

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
==============================================================================*/
#include <stdio.h>
#include "simpleParser.h"

simpleParser<100> ttycli(Serial);
byte debug = 0;

static const char PROGMEM cmds[] = 
#define CMD_HANDSHAKE 0
  "AYT "
#define CMD_VERSION 1
  "version "
#define CMD_SIGNATURE 2
  "signature "
#define CMD_GET_FUSE 3
  "fuse "
#define CMD_GET_CALIBRATE 4
  "calibrate "
#define CMD_READ_FLASH 5
  "read "
#define CMD_WRITE_FLASH 6
  "write "
#define CMD_SET_FUSE 7
  "setfuse "
#define CMD_ERASE 8
  "erase "
#define CMD_HELP 9
  "? "
  ;


#define PIN_VCC             LED_BUILTIN
#define PIN_12V             PIN_PE0
#define VPP12V_ON digitalWrite(PIN_12V, 0)
#define VPP12V_OFF digitalWrite(PIN_12V, 1)
#define PIN_RDY             PIN_PE1
#define PIN_OE              PIN_PA0
#define PIN_WR              PIN_PA1
#define PIN_BS1             PIN_PA2
#define PIN_BS2             LED_BUILTIN
#define PIN_XTAL1           PIN_PE2
#define PIN_XA0             PIN_PE3
#define PIN_XA1             PIN_PA6
#define PIN_PAGEL           LED_BUILTIN

#define PIN_D0              PIN_PD7
#define PIN_D1              PIN_PD6
#define PIN_D2              PIN_PD5
#define PIN_D3              PIN_PD4
#define PIN_D4              PIN_PD3
#define PIN_D5              PIN_PD2
#define PIN_D6              PIN_PD1
#define PIN_D7              PIN_PD0

#define PROG_ENABLE0        PIN_BS1
#define PROG_ENABLE1        PIN_XA0
#define PROG_ENABLE2        PIN_XA1
#define PROG_ENABLE3        PIN_PAGEL

#define loadAddressLow(a) loadAddress(LOW, a)
#define loadAddressHigh(a) loadAddress(HIGH, a)

#define loadDataLow(d) loadData(LOW, d)
#define loadDataHigh(d) loadData(HIGH, d)

byte lastCommand = 0xFF;

void setDataPinMode(unsigned char value)
{
  pinMode(PIN_D0, value);
  pinMode(PIN_D1, value);
  pinMode(PIN_D2, value);
  pinMode(PIN_D3, value);
  pinMode(PIN_D4, value);
  pinMode(PIN_D5, value);
  pinMode(PIN_D6, value);
  pinMode(PIN_D7, value);
  delayMicroseconds(5);
}

void setDataPins(uint8_t data) {
  digitalWrite(PIN_D0, (data >> 0) & 0x01);
  digitalWrite(PIN_D1, (data >> 1) & 0x01);
  digitalWrite(PIN_D2, (data >> 2) & 0x01);
  digitalWrite(PIN_D3, (data >> 3) & 0x01);
  digitalWrite(PIN_D4, (data >> 4) & 0x01);
  digitalWrite(PIN_D5, (data >> 5) & 0x01);
  digitalWrite(PIN_D6, (data >> 6) & 0x01);
  digitalWrite(PIN_D7, (data >> 7) & 0x01);
  delayMicroseconds(5);
}

unsigned char readDataPins()
{
  unsigned char retData;

  setDataPinMode(INPUT);
  digitalWrite(PIN_OE, LOW);
  delayMicroseconds(5);
  retData = (digitalRead(PIN_D0) << 0) & 0x01;
  retData |= (digitalRead(PIN_D1) << 1) & 0x02;
  retData |= (digitalRead(PIN_D2) << 2) & 0x04;
  retData |= (digitalRead(PIN_D3) << 3) & 0x08;
  retData |= (digitalRead(PIN_D4) << 4) & 0x10;
  retData |= (digitalRead(PIN_D5) << 5) & 0x20;
  retData |= (digitalRead(PIN_D6) << 6) & 0x40;
  retData |= (digitalRead(PIN_D7) << 7) & 0x80;
  digitalWrite(PIN_OE, HIGH);
  if (debug) printf("readDataPins %x\n", retData);
  return retData;
}

void initProgramMode()
{
  if (debug) printf("initProgramMode\n");
  setDataPinMode(INPUT);

  // Set Prog_enable pins to 0000.
  digitalWrite(PROG_ENABLE0, LOW);
  digitalWrite(PROG_ENABLE1, LOW);
  digitalWrite(PROG_ENABLE2, LOW);
  digitalWrite(PROG_ENABLE3, LOW);

  // Shutdown VCC and +12V
  digitalWrite(PIN_VCC, LOW);
  VPP12V_OFF;  // RESET

  delayMicroseconds(25);

  // Apply 5V to target device.
  digitalWrite(PIN_VCC, HIGH);
  digitalWrite(PIN_BS1, LOW);
  delayMicroseconds(50);

  // Apply +12V to target device.
  VPP12V_ON;

  // Wait before giving any parallel programming commands.
  delayMicroseconds(500);

  // Enable output and write modes.
  digitalWrite(PIN_OE, HIGH);
  digitalWrite(PIN_WR, HIGH);
  delayMicroseconds(50);
}

void resetProgramMode()
{
  if (debug) printf("resetProgramMode\n");
  // Power the device down by setting VCC and +12V to low.
  digitalWrite(PIN_VCC, LOW);
  VPP12V_OFF;

  // Reset command bits.
  digitalWrite(PIN_RDY, HIGH);
  digitalWrite(PIN_OE,  HIGH); // active low
  digitalWrite(PIN_WR, HIGH);  //WR is active low
  digitalWrite(PIN_BS1, LOW);
  digitalWrite(PIN_BS2, LOW);
  digitalWrite(PIN_XTAL1, LOW);
  digitalWrite(PIN_XA0, HIGH);  //idle
  digitalWrite(PIN_XA1, HIGH);
  digitalWrite(PIN_PAGEL, LOW);
  lastCommand = 0xFF;
}

void loadCommand(unsigned char cmd)
{
  if (debug) printf("load cmd %x\n", cmd);
  // Set XA1, XA0 to 1, 0.
  digitalWrite(PIN_XA0, LOW);
  digitalWrite(PIN_XA1, HIGH);

  // Set BS1 to 0.
  digitalWrite(PIN_BS1, LOW);

  // Set DATA with specified command.
  setDataPinMode(OUTPUT);
  setDataPins(cmd);
  // Pulse XTAL1.
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, LOW);
  delayMicroseconds(5);
  lastCommand = cmd;
}

void loadAddress(unsigned char seg, unsigned char addr)
{
  if (debug) printf("load address %x %x\n", seg, addr);
  // Set XA1, XA0 to 0, 0.
  digitalWrite(PIN_XA0, LOW);
  digitalWrite(PIN_XA1, LOW);

  // Set BS1 based on address HIGH or LOW.
  digitalWrite(PIN_BS1, seg);

  // Set DATA with specified address.
  setDataPinMode(OUTPUT);
  setDataPins(addr);

  // Pulse XTAL1.
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, LOW);
  delayMicroseconds(5);
}

void loadData(unsigned char seg, unsigned char data)
{
  if (debug) printf("load data %x %x\n", seg, data);

  // Set XA1, XA0 to 0, 1.
  digitalWrite(PIN_XA0, HIGH);
  digitalWrite(PIN_XA1, LOW);

  // Set BS1 based on data HIGH or LOW.
  digitalWrite(PIN_BS1, seg);

  // Set DATA.
  setDataPinMode(OUTPUT);
  setDataPins(data);

  // Pulse XTAL1.
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_XTAL1, LOW);
  delayMicroseconds(5);
}

unsigned char strobeWrite()
{
  unsigned char writeTimeout = 0;
  if (debug) printf("programPage\n");
  // Pulse WR.
  delayMicroseconds(5);
  digitalWrite(PIN_WR, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_WR, HIGH);

  // Wait until RDY/BSY goes high.
  while (writeTimeout < 250)
  {
    delay(10);
    writeTimeout++;

    if (digitalRead(PIN_RDY) == HIGH)
    {
      writeTimeout = 0;
      break;
    }
  }

  return (writeTimeout == 0) ? 0x30 : 0x31;
}

void readSignature(unsigned char* signature)
{
  unsigned char addrOffset = 0;

  // Enter device into programming mode.
  initProgramMode();

  // Load Command 0000 1000.
  loadCommand(0x08);

  while (addrOffset < 3)
  {
    // Load Address Low Byte for signature 0.
    loadAddressLow(addrOffset);

    // Set OE to 0, and BS1 to 0.
    //   digitalWrite(PIN_OE, LOW);
    digitalWrite(PIN_BS1, LOW);

    // Read signature byte from DATA.
    signature[addrOffset] = readDataPins();

    // Set OE to 1.
    digitalWrite(PIN_OE, HIGH);
    addrOffset++;
  }

  resetProgramMode();
}

void readFuseBytes(unsigned char* fuseConfig)
{
  // Enter device into programming mode.
  initProgramMode();

  // Load Command 0000 0100.
  loadCommand(0x04);

  // Reading Fuse Low byte. Set OE to 0, BS2 to 0 and BS1 to 0.
  //  digitalWrite(PIN_OE, LOW);
  digitalWrite(PIN_BS1, LOW);

  fuseConfig[0] = readDataPins();

  fuseConfig[1] = 0;
  fuseConfig[2] = 0;

  // Reading Lock byte. Set OE to 0, BS2 to 0 and BS1 to 1.
  //  digitalWrite(PIN_OE, LOW);
  digitalWrite(PIN_BS1, HIGH);
  fuseConfig[3] = readDataPins();

  resetProgramMode();
}

unsigned char setLockFuse(unsigned char fuseVal)
{
  unsigned char fuseSetTimeout = 0;

  // Enter device into programming mode.
  initProgramMode();

  // Load Command 0010 0000.
  loadCommand(0x20);

  // Load Data Low Byte.
  loadDataLow(fuseVal);

  // Give WR a negative pulse.
  delayMicroseconds(5);
  digitalWrite(PIN_WR, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_WR, HIGH);

  // Wait until RDY/BSY goes high.
  while (fuseSetTimeout < 250)
  {
    delay(20);
    fuseSetTimeout++;

    if (digitalRead(PIN_RDY) == HIGH)
    {
      fuseSetTimeout = 0;
      break;
    }
  }

  resetProgramMode();
  return (fuseSetTimeout == 0) ? 0x30 : 0x31;
}

unsigned char setFuseLow(unsigned char fuseVal)
{
  unsigned char fuseSetTimeout = 0;

  // Enter device into programming mode.
  initProgramMode();

  // Load Command 0100 0000.
  loadCommand(0x40);

  // Load Data Low Byte.
  loadDataLow(fuseVal);

  // Set BS1 to 0 and BS2 to 0.
  digitalWrite(PIN_BS1, LOW);
  digitalWrite(PIN_BS2, LOW);

  // Give WR a negative pulse.
  delayMicroseconds(5);
  digitalWrite(PIN_WR, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_WR, HIGH);

  // Wait until RDY/BSY goes high.
  while (fuseSetTimeout < 250)
  {
    delay(20);
    fuseSetTimeout++;

    if (digitalRead(PIN_RDY) == HIGH)
    {
      fuseSetTimeout = 0;
      break;
    }
  }

  resetProgramMode();
  return (fuseSetTimeout == 0) ? 0x30 : 0x31;
}

unsigned char readCalibrationByte()
{
  unsigned char tempData;

  // Enter device into programming mode.
  initProgramMode();

  // Load Command 0000 1000.
  loadCommand(0x08);

  // Load Address Low Byte 0x00.
  loadAddressLow(0x00);

  // Set OE to 0, and BS1 to 1.
  //  digitalWrite(PIN_OE, LOW);
  digitalWrite(PIN_BS1, HIGH);

  // Read calibration byte from DATA.
  tempData = readDataPins();

  // Set OE to 1.
  digitalWrite(PIN_OE, HIGH);
  resetProgramMode();

  return tempData;
}

void writeFlashByte(unsigned int addr, byte val)
{
  // Set read mode if last command is not CMD_WRITE_FLASH.
  if (lastCommand != 0x10) {
    // Enter device into programming mode.
    initProgramMode();

    // Load Command 0001 0000.
    loadCommand(0x10);
  }

  byte lowaddr = addr & 1;   // High or low word
  addr /= 2;         // turn byte address into word address

  // Load Address High Byte.
  loadAddressHigh(addr >> 8);

  // Load Address Low Byte.
  loadAddressLow(addr & 255);

  digitalWrite(PIN_BS1, lowaddr);

  loadData(lowaddr, val);
  strobeWrite();
  if (debug) printf("%04x: %02x\n", addr, val);
}

void readFlashByte(unsigned int addr)
{
  // Set read mode if last command is not CMD_READ_FLASH.
  if (lastCommand != 0x2) {
    // Enter device into programming mode.
    initProgramMode();
    // Load Command 0000 0010.
    loadCommand(0x02);
  }

  byte lowaddr = addr & 1;   // High or low word
  addr /= 2;         // turn byte address into word address

  // Load Address High Byte.
  loadAddressHigh(addr >> 8);

  // Load Address Low Byte.
  loadAddressLow(addr & 255);

  digitalWrite(PIN_BS1, lowaddr);

  // Read flash word low byte.
  byte data = readDataPins();
  printf(" %02x", data);
}

void dumpFlash(unsigned int addr) {
  printf("dumpFlash %x\n", addr);
  byte savedebug = debug;
  debug = 0;
  for (int j = 0; j < 8; j++) {
    printf("%04x: ", addr);
    for (int i = 0; i < 16; i++) {
      readFlashByte(addr++);
    }
    Serial.println();
  }
  debug = savedebug;
}

unsigned char eraseChip()
{
  unsigned char eraseTimeout = 0;
  printf("eraseChip\n");
  // Enter device into programming mode.
  initProgramMode();

  // Load Command 1000 0000.
  loadCommand(0x80);

  // Give WR a negative pulse.
  delayMicroseconds(5);
  digitalWrite(PIN_WR, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_WR, HIGH);

  // Wait until RDY/BSY goes high.
  while (eraseTimeout < 250)
  {
    delay(20);
    eraseTimeout++;

    if (digitalRead(PIN_RDY) == HIGH)
    {
      eraseTimeout = 0;
      break;
    }
  }

  resetProgramMode();
  return (eraseTimeout == 0) ? 0x30 : 0x31;
}


static FILE mystdout;
int addr, val;
byte lastcmd = 255;

void setup()
{
  // Set power pin modes.
  pinMode(PIN_VCC, OUTPUT);   // VCC/+5V output control.
  pinMode(PIN_12V, OUTPUT);   // +12V output control.

  // Set initial state of the power related I/O pins.
  digitalWrite(PIN_VCC, LOW);
  VPP12V_OFF;

  // Initialize serial interface.
  Serial.begin(115200);
  PORTMUX.USARTROUTEA = PORTMUX_USART0_ALT1_gc;
  VPORTA.DIR |= 1 << 4;
  delayMicroseconds(55);
  fdev_setup_stream(&mystdout, fput, NULL, _FDEV_SETUP_WRITE);
  stdout = &mystdout;

  // Sep pin modes and initial values.
  pinMode(PIN_RDY, INPUT);
  pinMode(PIN_OE, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_BS1, OUTPUT);
  pinMode(PIN_BS2, OUTPUT);
  pinMode(PIN_XTAL1, OUTPUT);
  pinMode(PIN_XA0, OUTPUT);
  pinMode(PIN_XA1, OUTPUT);
  pinMode(PIN_PAGEL, OUTPUT);

  setDataPinMode(OUTPUT);
  resetProgramMode();

  printf("\nHVPP ready!\n");
}


void loop()
{
  int clicmd;
  int16_t addr, val;
  uint8_t results[64];

  Serial.print("Cmd: ");
  ttycli.reset();
  ttycli.getLineWait();

  clicmd = ttycli.tryihex(&addr, results);
  if (clicmd > 0) {  // We have an intel hex file line?
    for (int i=0; i < clicmd; i++) {
      writeFlashByte(addr, results[i]);
      addr++;
    }
    return;                             /* next command */
  }
  // else try an "interactive" command.

  clicmd = ttycli.keyword(cmds); // look for a command.
  if (debug) printf("Have command %d\n", clicmd);
  switch (clicmd) {
    case CMD_HELP:
      Serial.println(F("HV programming tool\n"));
      Serial.println(F("Enter a command.  One of:"));
      Serial.println(reinterpret_cast<const __FlashStringHelper *>(cmds));
      Serial.println();
      break;

  case CMD_VERSION:
  case CMD_HANDSHAKE:
    printf("\nATtiny28 High Voltage Parallel Programmer\nVersion 0.1\n");
    break;

  case CMD_SIGNATURE:
    // Read chip 3 byte chip signature.
    readSignature(results);
    printf("Signature: %02x%02x%02x\n", results[0],
           results[1], results[2]);
    break;

  case CMD_GET_FUSE:
    // Read low, high, extended and lock fuse bytes.
    readFuseBytes(results);
    printf("Fuse: %02x  Lock: %02x\n", results[0], results[1] );
    break;

  case CMD_GET_CALIBRATE:
    val = readCalibrationByte();
    printf("Calibration %02x\n", val);
    break;

  case CMD_WRITE_FLASH:
    addr = ttycli.number();
    while (1) {
      val = ttycli.number();
      if (val < 0)
        break;
      writeFlashByte(addr, val);
      addr++;
    }
    break;

  case CMD_READ_FLASH:
    addr = ttycli.number();
    if (addr >= 0) {
      dumpFlash(addr);
    }
    break;

  case CMD_ERASE:
    // Perform chip erase.
    val = eraseChip();
    printf("Erased %s\n", val ? "OK" : "Failed");
    break;

  case CMD_SET_FUSE:
    val = ttycli.number();
    if (val > 0) {
      val = setFuseLow(val);
      printf("Fuse %s\n", val ? "OK" : "Failed");
    }
    break;
  }
  resetProgramMode();
}

#include <stdio.h>
static int fput(char c, FILE * f __attribute__((unused))) {
  Serial.write( c);
  return 0;
}
