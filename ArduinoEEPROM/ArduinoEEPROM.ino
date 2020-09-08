// ArduinoEEPROM
//
// Copyright 2020 by Bill Westfield
// This is stripped version of ArduinoISP designed to allow access to the EEPROM of
//  AVR that is actually running the sketch, using the STK500v1 or Arduino Protocols.
// This is needed because the normal Arduino Bootloader does not support accesss to
//  the EEPROM.  The usual methods involve using a programmer, or specialized software
//  customized to the particular sketch that will use the EEPROM.
// This sketch will allow the EEPROM to be programmed using standard methods, and then
//  the sketch that uses it can be uploaded.
// Also note avrdude's "interactive" mode ("-t")
//
// There is code in here that doesn't do anything useful.
//  You can read flash memory (why would you want to?)
//  Attempts to write to flash memory are ignored.
//
// Based on ArduinoISP, which is:
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
// http://www.opensource.org/licenses/bsd-license.php
//

//
// Put an LED (with resistor) on the following pins:
// 9: Heartbeat   - shows the programmer is running
// 8: Error       - Lights up if something goes wrong (use red if that makes sense)
// 7: Programming - In communication with the slave
//

#include "Arduino.h"
#include <avr/boot.h>

#undef SERIAL


#define PROG_FLICKER true


// Configure which pins to use:

// The standard pin configuration.

#define LED_HB    	7
#define LED_ERR   	13
#define LED_PMODE 	5

// Configure the serial port to use.
//
// Prefer the USB virtual serial port (aka. native USB port), if the Arduino has one:
//   - it does not autoreset (except for the magic baud rate of 1200).
//   - it is more reliable because of USB handshaking.
//
// Leonardo and similar have an USB virtual serial port: 'Serial'.
// Due and Zero have an USB virtual serial port: 'SerialUSB'.
//
// On the Due and Zero, 'Serial' can be used too, provided you disable autoreset.
// To use 'Serial': #define SERIAL Serial

#ifdef SERIAL_PORT_USBVIRTUAL
#define SERIAL SERIAL_PORT_USBVIRTUAL
#else
#define SERIAL Serial
#endif


// Configure the baud rate:

#define BAUDRATE	19200
// #define BAUDRATE	115200
// #define BAUDRATE	1000000


#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...

void pulse(int pin, int times);

void flush() {
  Serial.flush();
  delay(200);
  while (Serial.read() > 0 )
    ;
  delay(100);
}

void setup() {
  SERIAL.begin(BAUDRATE);

  pinMode(LED_PMODE, OUTPUT);
  pulse(LED_PMODE, 2);
  pinMode(LED_ERR, OUTPUT);
  pulse(LED_ERR, 2);
  pinMode(LED_HB, OUTPUT);
  pulse(LED_HB, 2);
  flush();

}

int error = 0;
int pmode = 0;
// address for reading and writing, set by 'U' command
unsigned int here;
uint8_t buff[256]; // global block storage
uint8_t sigbytes[3] = {
  SIGNATURE_0,
  SIGNATURE_1,
  SIGNATURE_2
};

#define beget16(addr) (*addr * 256 + *(addr+1) )
typedef struct param {
  uint8_t devicecode;
  uint8_t revision;
  uint8_t progtype;
  uint8_t parmode;
  uint8_t polling;
  uint8_t selftimed;
  uint8_t lockbytes;
  uint8_t fusebytes;
  uint8_t flashpoll;
  uint16_t eeprompoll;
  uint16_t pagesize;
  uint16_t eepromsize;
  uint32_t flashsize;
}
parameter;

parameter param;

// this provides a heartbeat on pin 9, so you can tell the software is running.
uint8_t hbval = 128;
int8_t hbdelta = 8;
void heartbeat() {
  static unsigned long last_time = 0;
  unsigned long now = millis();
  if ((now - last_time) < 40)
    return;
  last_time = now;
  if (hbval > 192) hbdelta = -hbdelta;
  if (hbval < 32) hbdelta = -hbdelta;
  hbval += hbdelta;
  analogWrite(LED_HB, hbval);
}

static bool rst_active_high;

void loop(void) {
  // is pmode active?
  if (pmode) {
    digitalWrite(LED_PMODE, HIGH);
  } else {
    digitalWrite(LED_PMODE, LOW);
  }
  // is there an error?
  if (error) {
    digitalWrite(LED_ERR, HIGH);
  } else {
    digitalWrite(LED_ERR, LOW);
  }

  // light the heartbeat LED
  heartbeat();
  if (SERIAL.available()) {
    avrisp();
  }
}

uint8_t getch() {
  while (!SERIAL.available());
  return SERIAL.read();
}
void fill(int n) {
  for (int x = 0; x < n; x++) {
    buff[x] = getch();
  }
}

#define PTIME 30
void pulse(int pin, int times) {
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  } while (times--);
}

void prog_lamp(int state) {
  if (PROG_FLICKER) {
    digitalWrite(LED_PMODE, state);
  }
}

void empty_reply() {
  if (CRC_EOP == getch()) {
    SERIAL.print((char)STK_INSYNC);
    SERIAL.print((char)STK_OK);
  } else {
    error++;
    SERIAL.print((char)STK_NOSYNC);
  }
}

void breply(uint8_t b) {
  if (CRC_EOP == getch()) {
    SERIAL.print((char)STK_INSYNC);
    SERIAL.print((char)b);
    SERIAL.print((char)STK_OK);
  } else {
    error++;
    SERIAL.print((char)STK_NOSYNC);
  }
}

void get_version(uint8_t c) {
  switch (c) {
    case 0x80:
      breply(HWVER);
      break;
    case 0x81:
      breply(SWMAJ);
      break;
    case 0x82:
      breply(SWMIN);
      break;
    case 0x93:
      breply('S'); // serial programmer
      break;
    default:
      breply(0);
  }
}

void set_parameters() {
  // call this after reading parameter packet into buff[]
  param.devicecode = buff[0];
  param.revision   = buff[1];
  param.progtype   = buff[2];
  param.parmode    = buff[3];
  param.polling    = buff[4];
  param.selftimed  = buff[5];
  param.lockbytes  = buff[6];
  param.fusebytes  = buff[7];
  param.flashpoll  = buff[8];
  // ignore buff[9] (= buff[8])
  // following are 16 bits (big endian)
  param.eeprompoll = beget16(&buff[10]);
  param.pagesize   = beget16(&buff[12]);
  param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  param.flashsize = buff[16] * 0x01000000
                    + buff[17] * 0x00010000
                    + buff[18] * 0x00000100
                    + buff[19];

  // AVR devices have active low reset, AT89Sx are active high
  rst_active_high = (param.devicecode >= 0xe0);
}

void start_pmode() {
  pmode = 1;
}

void end_pmode() {
  pmode = 0;
}

void universal() {
  uint16_t a;
  uint8_t ch;
  fill(4);
  switch (buff[0]) {
    case 0x30: // signature byte N
      breply(sigbytes[buff[2]]);
      return;
    case 0x50:  // fuses
      if (buff[1] == 0 && buff[2] == 0) {
        breply(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS));
        return;
      } else if (buff[1] == 8) {
        breply(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS));
        return;
      }
      break;
    case 0x58: // other fuses
      if (buff[1] == 8) {
        breply(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS));
        return;
      }
      break;
    case 0xA0: // read eeprom
      a = (buff[1] << 8) + buff[2];
      breply(eeprom_read_byte(a));
      return;
    case 0xC0: // read eeprom
      a = (buff[1] << 8) + buff[2];
      eeprom_write_byte(a, buff[3]);
      breply(buff[3]);
      return;
  }
  breply(0);
}

void commit(unsigned int addr) {
  if (PROG_FLICKER) {
    prog_lamp(LOW);
  }
  if (PROG_FLICKER) {
    delay(PTIME);
    prog_lamp(HIGH);
  }
}

unsigned int current_page() {
  if (param.pagesize == 32) {
    return here & 0xFFFFFFF0;
  }
  if (param.pagesize == 64) {
    return here & 0xFFFFFFE0;
  }
  if (param.pagesize == 128) {
    return here & 0xFFFFFFC0;
  }
  if (param.pagesize == 256) {
    return here & 0xFFFFFF80;
  }
  return here;
}

void halt(uint32_t d) {
  while (1) {
    digitalWrite(LED_ERR, 0);
    delay(d);
    digitalWrite(LED_ERR, 1);
    delay(d);
  }
}

#define EECHUNK (32)
uint8_t write_eeprom(unsigned int length) {
  // here is a word address, get the byte address
  unsigned int start = here * 2;
  unsigned int remaining = length;
  if (length > param.eepromsize) {
    error++;
    return STK_FAILED;
  }
  while (remaining > EECHUNK) {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
  }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}

// write (length) bytes, (start) is a byte address
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length) {
  // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
  fill(length);
  prog_lamp(LOW);
  for (unsigned int x = 0; x < length; x++) {
    unsigned int addr = start + x;
    eeprom_write_byte((uint8_t *)addr, buff[x]);
    delay(45);
  }
  prog_lamp(HIGH);
  return STK_OK;
}

void program_page() {
  char result = (char) STK_FAILED;
  unsigned int length = 256 * getch();
  length += getch();
  char memtype = getch();
  // flash memory @here, (length) bytes
  if (memtype == 'F') {
    halt(200);
    return;
  }
  if (memtype == 'E') {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getch()) {
      SERIAL.print((char) STK_INSYNC);
      SERIAL.print(result);
    } else {
      error++;
      SERIAL.print((char) STK_NOSYNC);
    }
    return;
  }
  SERIAL.print((char)STK_FAILED);
  return;
}


char eeprom_read_page(int length) {
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    uint8_t ee = eeprom_read_byte((const uint8_t *)addr);
    SERIAL.print((char) ee);
  }
  return STK_OK;
}

char flash_read_page(int length) {
  for (int x = 0; x < length; x ++) {
    uint8_t b = pgm_read_byte(here);
    SERIAL.print((char) b);
    here++;
  }
  return STK_OK;
}

void read_page() {
  char result = (char)STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  if (CRC_EOP != getch()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  SERIAL.print(result);
}

void read_signature() {
  if (CRC_EOP != getch()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  SERIAL.print((char) SIGNATURE_0);
  SERIAL.print((char) SIGNATURE_1);
  SERIAL.print((char) SIGNATURE_2);
  SERIAL.print((char) STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////


////////////////////////////////////
////////////////////////////////////
void avrisp() {
  uint8_t ch = getch();
  switch (ch) {
    case '0': // signon
      error = 0;
      empty_reply();
      break;
    case '1':
      if (getch() == CRC_EOP) {
        SERIAL.print((char) STK_INSYNC);
        SERIAL.print("AVR ISP");
        SERIAL.print((char) STK_OK);
      }
      else {
        error++;
        SERIAL.print((char) STK_NOSYNC);
      }
      break;
    case 'A':
      get_version(getch());
      break;
    case 'B':
      fill(20);
      set_parameters();
      empty_reply();
      break;
    case 'E': // extended parameters - ignore for now
      fill(5);
      empty_reply();
      break;
    case 'P':
      if (!pmode)
        start_pmode();
      empty_reply();
      break;
    case 'U': // set address (word)
      here = getch();
      here += 256 * getch();
      empty_reply();
      break;

    case 0x60: //STK_PROG_FLASH
      getch(); // low addr
      getch(); // high addr
      empty_reply();
      break;
    case 0x61: //STK_PROG_DATA
      getch(); // data
      empty_reply();
      break;

    case 0x64: //STK_PROG_PAGE
      program_page();
      break;

    case 0x74: //STK_READ_PAGE 't'
      read_page();
      break;

    case 'V': //0x56
      universal();
      break;
    case 'Q': //0x51
      error = 0;
      end_pmode();
      empty_reply();
      break;

    case 0x75: //STK_READ_SIGN 'u'
      read_signature();
      break;

    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case CRC_EOP:
      error++;
      SERIAL.print((char) STK_NOSYNC);
      break;

    // anything else we will return STK_UNKNOWN
    default:
      error++;
      if (CRC_EOP == getch())
        SERIAL.print((char)STK_UNKNOWN);
      else
        SERIAL.print((char)STK_NOSYNC);
  }
}
