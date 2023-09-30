/*
 * Single NeoPixel on 48MHz SAMD21 (eg Adafruit QT Py)
 * Sep 2023 by William Westfield (WestfW) - Released to the public domain.
 *
 * API Provided:
 *  singleNeo(pin, red, green, blue);
 *    Sets the pin to output and lights it up with the specified RGB value.
 *
 * https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 * A newpixel waveform is about 1299ns long.  It starts with a HIGH
 * signal of about 350ns, has a 2nd ~350ns that matches the data
 * value, ends with a low signal of about 500ns, and ends with a low of
 * about 500ns.
 */

//
//-----------------------------------------------------------------
//
#if defined(ARDUINO_ARCH_SAMD) && (F_CPU == 48000000L)
extern "C" {void delay300(); void delay800();};
asm("delay800: push {lr}\n"
    "        bl delay300\n"
    "        bl delay300\n"
    "        pop {pc}\n"
    "delay300: push {lr}; nop; nop; pop {pc}\n");

#define _singleneo_setupIO(pin)				\
  PortGroup *neoport = digitalPinToPort(pin);			\
  uint32_t pinMask = 1ul << g_APinDescription[pin].ulPin;

#define _singleneo_setbit     neoport->OUTSET.reg = pinMask
#define _singleneo_clrbit     neoport->OUTCLR.reg = pinMask

// Adafruit QT Py M0
#define NEOPIN 11
//
//-----------------------------------------------------------------
//
#elif (ARDUINO_ARCH_RP2040)

// these delay functions assume that SysTick won't wrap, which is ensured
// in the setupIO function by making sure there is enough time before wrap
// to send out all 24 bits.
void delay800() {
  uint32_t startt = SysTick->VAL;
  while ((startt - SysTick->VAL) < (uint32_t)(800e-9/(1.0/F_CPU)))
    ;
}
void delay300() {
  uint32_t startt = SysTick->VAL;
  while ((startt - SysTick->VAL) < (uint32_t)(300e-9/(1.0/F_CPU)))
    ;
}

#define _singleneo_setupIO(pin)				\
  while (SysTick->VAL < (uint32_t) (100*1200e-9*F_CPU)) ;	\
  auto neoport =  sio_hw;					\
  uint32_t pinmask = 1<<pin


#define _singleneo_setbit neoport->gpio_set = pinmask
#define _singleneo_clrbit  neoport->gpio_clr = pinmask

// Cytron Maker Pi
#define NEOPIN 18
#endif
//
//-----------------------------------------------------------------
//
// Support function
static void inline _singleNeoSendBytes (uint32_t pin, uint8_t *ptr) {
  uint8_t p, *end = ptr+3;
 
  _singleneo_setupIO(pin);
  p = *ptr++;
  
  uint32_t bitMask = 0x80;
  noInterrupts();
  for (;;) {
    _singleneo_setbit;
    delay300();
    if (p & bitMask) {
      delay800();
      _singleneo_clrbit;
    } else {
      _singleneo_clrbit;
      delay800();
    }
    if (bitMask >>= 1) {
      delay300();
      asm("nop;nop;nop;nop;");
    } else {
      if (ptr >= end)
	break;
      p = *ptr++;
      bitMask = 0x80;
    }
  }
  interrupts();
}

// The main API - set the (singe or first) neopixel on the given pin
// to the provided R, G, B values.

void singleNeo(int pin, int redv, int greenv, int bluev) {
  uint8_t vals[3] = {(uint8_t)greenv, (uint8_t)redv, (uint8_t)bluev};
  pinMode(pin, OUTPUT);
  _singleNeoSendBytes(pin, vals);
}

//
//-----------------------------------------------------------------
// Skeleton program

void setup() {
  delay(500);
}

void loop() {
  singleNeo(NEOPIN, 0, 0, 0);  // off
  delay(1000);
  singleNeo(NEOPIN, 30, 0, 0);  // red
  delay(1000);
  singleNeo(NEOPIN, 0, 30, 0); // green
  delay(1000);
  singleNeo(NEOPIN, 0, 0, 30); // blue
  delay(3000);
}
