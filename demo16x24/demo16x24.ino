/***********************************************************************
 * demo16x24.c - Arduino demo program for Holtek HT1632 LED driver chip,
 *            As implemented on the Sure Electronics DE-DP016 display board
 *            (16*24 dot matrix LED module.)
 * Nov, 2008 by Bill Westfield ("WestfW")
 *   Copyrighted and distributed under the terms of the Berkely license
 *   (copy freely, but include this notice of original author.)
 ***********************************************************************/

#include "ht1632.h"
#include <avr/pgmspace.h>
#include "font.h"

#define X_MAX 23
#define Y_MAX 15

#define DEBUGPRINT(fmt, args...) // do nothing
#define ASSERT(condition) // do nothing

/*
 * Set these constants to the values of the pins connected to the SureElectronics Module
 */
static const byte ht1632_data = 10;  // Data pin (pin 7)
static const byte ht1632_wrclk = 11; // Write clock pin (pin 5)
static const byte ht1632_cs = 12;    // Chip Select (1, 2, 3, or 4)

// There should also be a common GND.
// The module with all LEDs like draws about 200mA,
//  which makes it PROBABLY powerable via Arduino +5V


#define DEMOTIME 10000  // 30 seconds max on each demo is enough.
#define DISPDELAY 40    // Each "display" lasts this long
#define LONGDELAY 1000  // This delay BETWEEN demos


/***********************************************************************
 * ht1632_chipselect / ht1632_chipfree
 * Select or de-select a particular ht1632 chip.
 * De-selecting a chip ends the commands being sent to a chip.
 * CD pins are active-low; writing 0 to the pin selects the chip.
 ***********************************************************************/

void ht1632_chipselect(byte chipno) 
{
  DEBUGPRINT("\nHT1632(%d) ", chipno);
  digitalWrite(chipno, 0);   // select the chip
}

void ht1632_chipfree(byte chipno)
{
  DEBUGPRINT(" [done %d]", chipno);
  digitalWrite(chipno, 1);  // unselect teh chip
}


/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk
 * Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent
 * being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632_writebits (byte bits, byte firstbit)
{
  DEBUGPRINT(" ");
  while (firstbit) {
    DEBUGPRINT((bits&firstbit ? "1" : "0"));
    digitalWrite(ht1632_wrclk, LOW);
    if (bits & firstbit) {
      digitalWrite(ht1632_data, HIGH);
    } 
    else {
      digitalWrite(ht1632_data, LOW);
    }
    digitalWrite(ht1632_wrclk, HIGH);
    firstbit >>= 1;
  }
}

/*
 * ht1632_sendcmd
 * Send a command to the ht1632 chip.
 * A command consists of a 3-bit "CMD" ID, an 8bit command, and
 * one "don't care bit".
 *   Select 1 0 0 c7 c6 c5 c4 c3 c2 c1 c0 xx Free
 */
static void ht1632_sendcmd (byte command)
{
  ht1632_chipselect(ht1632_cs);  // Select chip
  ht1632_writebits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);  // send the actual command
  ht1632_writebits(0, 1); 	/* one extra dont-care bit in commands. */
  ht1632_chipfree(ht1632_cs); //done
}

/*
 * ht1632_senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
static void ht1632_senddata (byte address, byte data)
{
  ht1632_chipselect(ht1632_cs);  // Select chip
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6); // Send address
  ht1632_writebits(data, 1<<3); // send 4 bits of data
  ht1632_chipfree(ht1632_cs); // done
}

/*
 * ht1632_setup
 * Initialize the chip as appropriate for our system by sending a
 * set of configuration commands.
 */

void ht1632_setup()
{
  pinMode(ht1632_cs, OUTPUT);
  digitalWrite(ht1632_cs, HIGH); 	/* unselect (active low) */
  pinMode(ht1632_wrclk, OUTPUT);
  pinMode(ht1632_data, OUTPUT);
  ht1632_sendcmd(HT1632_CMD_SYSDIS);  // Disable system
  ht1632_sendcmd(HT1632_CMD_COMS11);  // set external driver topology (16*32, PMOS drivers)
  ht1632_sendcmd(HT1632_CMD_MSTMD); 	/* set Master Mode */
  ht1632_sendcmd(HT1632_CMD_SYSON); 	/* turn System on */
  ht1632_sendcmd(HT1632_CMD_LEDON); 	/* turn on LEDs */
  for (byte i=0; i<96; i++)
    ht1632_senddata(i, 0);  // clear the display!
  delay(LONGDELAY);
}

/*
 * we keep a copy of the display controller contents so that we can
 * know which bits are on without having to (slowly) read the device.
 * Note that we only use the low four bits of the shadow ram, since
 * we're shadowing 4-bit memory.  This makes things faster, and we
 * use the other half for a "snapshot" when we want to plot new data
 * based on older data...
 */
byte ht1632_shadowram[96];  // our copy of the display's RAM

/*
 * plot a point on the display, with the upper left hand corner
 * being (0,0), and the lower right hand corner being (23, 15).
 * Note that Y increases going "downward" in contrast with most
 * mathematical coordiate systems, but in common with many displays
 * No error checking; bad things may happen if arguments are out of
 * bounds!  (The ASSERTS compile to nothing by default
 */
void ht1632_plot (byte x, byte y, byte val)
{
  byte addr, bitval;

  ASSERT(x >= 0);
  ASSERT(x <= X_MAX);
  ASSERT(y >= 0);
  ASSERT(y <= y_MAX);

  /*
   * The 4 bits in a single memory word go DOWN, with the LSB
   * (first transmitted) bit being on top.  However, writebits()
   * sends the MSB first, so we have to do a sort of bit-reversal
   * somewhere.  Here, this is done by shifting the single bit in
   * the opposite direction from what you might expect.
   */
  bitval = 8>>(y&3);  // compute which bit will need set
  addr = (x<<2) + (y>>2);  // compute which memory word this is in
  if (val) {  // Modify the shadow memory
    ht1632_shadowram[addr] |= bitval;
  } 
  else {
    ht1632_shadowram[addr] &= ~bitval;
  }
  // Now copy the new memory value to the display
  ht1632_senddata(addr, ht1632_shadowram[addr]);
}

/*
 * get_shadowram
 * return the value of a pixel from the shadow ram.
 */
byte get_shadowram(byte x, byte y)
{
  byte addr, bitval;

  bitval = 8>>(y&3);  // compute which bit will need set (low bits)
  addr = (x<<2) + (y>>2);  // compute which memory word this is in
  if (ht1632_shadowram[addr] & bitval)
    return 1;
  return 0;
}


/*
 * snapshot_shadowram
 * Copy the shadow ram into the snapshot ram (the upper bits)
 * This gives us a separate copy so we can plot new data while
 * still having a copy of the old data.  snapshotram is NOT
 * updated by the plot functions (except "clear")
 */
void snapshot_shadowram()
{
  for (byte i=0; i< sizeof ht1632_shadowram; i++) {
    ht1632_shadowram[i] = (ht1632_shadowram[i] & 0x0F) | ht1632_shadowram[i] << 4;  // Use the upper bits
  }
}

/*
 * get_snapshotram
 * get a pixel value from the snapshot ram (instead of
 * the actual displayed (shadow) memory
 */
byte get_snapshotram(byte x, byte y)
{
  byte addr, bitval;

  bitval = 128>>(y&3);  // use upper bits!
  addr = (x<<2) + (y>>2);  // compute which memory word this is in
  if (ht1632_shadowram[addr] & bitval)
    return 1;
  return 0;
}

/*
 * ht1632_clear
 * clear the display, and the shadow memory, and the snapshot
 * memory.  This uses the "write multiple words" capability of
 * the chipset by writing all 96 words of memory without raising
 * the chipselect signal.
 */
void ht1632_clear()
{
  byte i;

  ht1632_chipselect(ht1632_cs);  // Select chip
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(0, 1<<6); // Send address
  for (i = 0; i < 96/2; i++) // Clear entire display
    ht1632_writebits(0, 1<<7); // send 8 bits of data
  ht1632_chipfree(ht1632_cs); // done
  for (i=0; i < 96; i++)
    ht1632_shadowram[i] = 0;
}

/***********************************************************************
 * plotting routines built on top of "dot" capabiliy of ht1632_plot().
 * Code past here is not particularly dependent on the underlying
 * graphics hardware
 ***********************************************************************/

#define plot(x,y,v)  ht1632_plot(x,y,v)
#define cls          ht1632_clear

/*
 * Draw a line between two points using the bresenham algorithm.
 * This particular bit of code is copied nearly verbatim from
 *   http://www.gamedev.net/reference/articles/article1275.asp
 * I don't like it much (too many local variables!), but it works,
 * and is fully explained at the site, so...
 */
void bres_line(byte x1, byte y1,   byte x2, byte y2,  byte val )
{
  char deltax = abs(x2 - x1);        // The difference between the x's
  char deltay = abs(y2 - y1);        // The difference between the y's
  char x = x1;                       // Start x off at the first pixel
  char y = y1;                       // Start y off at the first pixel
  char xinc1, xinc2, yinc1, yinc2, den, num, numadd, numpixels, curpixel;

  if (x2 >= x1) {                // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
  }  
  else {                          // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    plot(x, y, val);             // Draw the current pixel
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}


/*
 * Copy a character glyph from the myfont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 */
void ht1632_putchar(byte x, byte y, char c)
{
  byte dots;
  /*
   * This routine accepts ascii, but we only implement A-Z and 0-9,
   * so we first do some mapping to the correct range.
   */
  if (((c >= 'A') && (c <= 'Z')) ||
    (c >= 'a' && c <= 'z') ) {
    c &= 0x1F;   // A-Z maps to 1-26
  } 
  else if (c >= '0' && c <= '9') {
    c = (c - '0') + 27;
  } 
  else if (c == ' ') {
    c = 0; // space
  }
  for (byte col=0; col< 5; col++) {
    dots = pgm_read_byte_near(&myfont[c][col]);  // get a column worth of dots.
    for (byte row=0; row < 7; row++) {  // display each dot in the column.
      if (dots & (64>>row))   	     // only 7 rows.
        plot(x+col, y+row, 1);
      else 
        plot(x+col, y+row, 0);
    }
  }
}

/***********************************************************************
 * demo routines that are not particularly ht1632 dependent
 * These are built on top of the graphics primitives above
 ***********************************************************************/

/*
 * demo_scan()
 * scan all the leds one at a time,
 * using the plot() function, so as to demonstate the limits
 * of "plot" performance.
 */
void demo_scan ()
{
  byte x,y, bits;
  for (y=0; y<=Y_MAX; y++) {
    for (x=0; x <= X_MAX; x++) {
      plot(x,y,1);
      delay(DISPDELAY/2);
      plot(x,y,0);
    }
  }
}

/*
 * demo_random_walk()
 * have a single LED walk all over the display, randomly.
 */
void demo_random_walk ()
{
  byte x,y, dx, dy;
  dx = dy = 1;
  byte change;

  cls();
  for (int i=0; i < (DEMOTIME/DISPDELAY);  i++) {
    plot(x,y,1);  // draw a point
    delay(DISPDELAY); // wait a bit.
    change = random(1,32);
    /*
     * figure out where to go next.  This code is a bit
     * random in more senses than one, but it seems to
     * have results that are more or less what I had in
     * mind for this portion of the demo.
     */
    if (change < 9) {
      // do nothing
    } 
    else if (change == 10) {
      dx = -dx;
    } 
    else if (change == 11) {
      dy = -dy;
    } 
    else if (change == 12) {
      dx = dy = 0;
    } 
    else if (change == 13) {
      dx = dy = 1;
    } 
    else if (change == 14) {
      dx = dy = -1;
    }

    plot(x,y,0); // erase the point

    x = x + dx;
    y = y+ dy;
    if (x > X_MAX) {
      dx = -1;
      x = X_MAX;
    } 
    else if (x < 0) {
      x = 0;
      dx = 1;
    }
    if (y >  Y_MAX) {
      y = Y_MAX;
      dy = -1;
    } 
    else if (y < 0) {
      y = 0;
      dy = 1;
    }
  }
}


/*
 * demo_cross
 * Draw a crpss on the display, and then use the PWM function
 * to demonstate how well (or not) the ht1632 dimming function works.
 */
void demo_cross ()
{
  byte x, y;
  char intensity;

  for (x=0; x <= X_MAX; x++) {
    plot(x,7,1);
  }
  for (y=0; y <= Y_MAX; y++) {
    plot(12,y,1);
  }
  for (intensity=14; intensity >= 0; intensity--) {
    ht1632_sendcmd(HT1632_CMD_PWM + intensity);
    delay(LONGDELAY/4);
  }
  ht1632_sendcmd(HT1632_CMD_PWM + 15); // back to max brightness
}


/*
 * demo_bouncyline
 * Do the classic "bouncing line" demo, where the endpoints of a line
 * move independently and bounce off the edges of the display.
 * This should demonstrate (more or less) the performance limits of
 * the line drawing function.
 */
void demo_bouncyline ()
{
  char x1,y1, x2,y2, dx1, dy1, dx2, dy2;

  cls();
  x1 = random(0,X_MAX);
  x2 = random(0,X_MAX);
  y1 = random(0,Y_MAX);
  y2 = random(0,Y_MAX);
  dx1 = random(1,4);
  dx2 = random(1,4);
  dy1 = random(1,4);
  dy2 = random(1,4);
  for (int i=0; i < DEMOTIME/DISPDELAY; i++) {
    bres_line(x1,y1, x2,y2, 1);
    delay(DISPDELAY);
    bres_line(x1,y1, x2,y2, 0);

    x1 += dx1;
    if (x1 > X_MAX) {
      x1 = X_MAX;
      dx1 = -random(1,4);
    } 
    else if (x1 < 0) {
      x1 = 0;
      dx1 = random(1,4);
    }

    x2 += dx2;
    if (x2 > X_MAX) {
      x2 = X_MAX;
      dx2 = -random(1,4);
    } 
    else if (x2 < 0) {
      x2 = 0;
      dx2 = random(1,4);
    }

    y1 += dy1;
    if (y1 > Y_MAX) {
      y1 = Y_MAX;
      dy1 = -random(1,3);
    } 
    else if (y1 < 0) {
      y1 = 0;
      dy1 = random(1,3);
    }

    y2 += dy2;
    if (y2 > Y_MAX) {
      y2 = Y_MAX;
      dy2 = -random(1,3);
    } 
    else if (y2 < 0) {
      y2 = 0;
      dy2 = random(1,3);
    }
  }
}


/*
 * demo_rowbrightness
 * draw various lengths of horizontal lines, so that the
 * brightness variations as we stress the ht1632 drivers
 * are apparent.
 */
void demo_rowbrightness ()
{
  for (byte y = 8; y <= Y_MAX; y++) {
    bres_line(0,y, y+6, y, 1);
  }
  delay(LONGDELAY*4);
}


/*
 * demo_life
 * Run the "life" game for a while, demonstrating the
 * ability of the AVR to update every pixle of the display
 * after having done some computation to figure out the new
 * value.  Also demonstrates the use of the snapshot ram.
 */
void demo_life ()
{
  byte x,y, neighbors, newval;

  plot(10,9,1);  // Plant an "acorn"; a simple pattern that
  plot(12,10,1); //  grows for quite a while..
  plot(9,11,1);
  plot(10,11,1);
  plot(13,11,1);
  plot(14,11,1);
  plot(15,11,1);

  delay(LONGDELAY);   // Play life
  snapshot_shadowram();

  for (int i=0; i < (DEMOTIME/DISPDELAY)/4; i++) {
    for (x=1; x < X_MAX; x++) {
      for (y=1; y < Y_MAX; y++) {
        neighbors = get_snapshotram(x, y+1) +
          get_snapshotram(x, y-1) +
          get_snapshotram(x+1, y) +
          get_snapshotram(x+1, y+1) +
          get_snapshotram(x+1, y-1) +
          get_snapshotram(x-1, y) +
          get_snapshotram(x-1, y+1) +
          get_snapshotram(x-1, y-1);

        switch (neighbors) {
        case 0:
        case 1:
          newval = 0;   // death by loneliness
          break;
        case 2:
          newval = get_snapshotram(x,y);
          break;  // remains the same
        case 3:
          newval = 1;
          break;
        default:
          newval = 0;  // death by overcrowding
          break;
        }

        plot(x,y, newval);
      }
    }
    snapshot_shadowram();
    delay(DISPDELAY);
  }
  delay(1000);
}


/*
 * demo_chars
 * draw some characters on the display and scroll them vertically.
 */
void demo_chars ()
{

  cls();

  for (byte y=0; y <= Y_MAX-7; y++) {
    bres_line(0, y-1, X_MAX, y-1, 0); // erase top of old chars
    ht1632_putchar(0, y, 'A');
    ht1632_putchar(6, y, 'B');
    ht1632_putchar(12, y, '1');
    ht1632_putchar(18, y, '3');
    delay(DISPDELAY);
  }
  delay(LONGDELAY*5);
}


/*
 * Falling snowflakes demo by Andrew (Arduino forums)
 * See message #20 in thread http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1225239439
 */

#define NUMFLAKES 7
byte flakes[NUMFLAKES][2];

void demo_snowflakes() {

  for (int i = 0; i < NUMFLAKES; i++) {
    flakes[i][0] = random(X_MAX + 1);
    flakes[i][1] = random(Y_MAX + 1);
  }
  for (int time=0; time <= DEMOTIME/60; time++) {  // divisor determined by trial and error
    // Display flakes
    for (int i = 0; i < NUMFLAKES; i++) {
      plot(flakes[i][0], flakes[i][1]-1, 1);
      plot(flakes[i][0]+1, flakes[i][1], 1);
      plot(flakes[i][0], flakes[i][1]+1, 1);
      plot(flakes[i][0]-1, flakes[i][1], 1);
    }
    delay(DISPDELAY * 2);

    // Erase flakes
    for (int i = 0; i < NUMFLAKES; i++) {
      plot(flakes[i][0], flakes[i][1]-1, 0);
      plot(flakes[i][0]+1, flakes[i][1], 0);
      plot(flakes[i][0], flakes[i][1]+1, 0);
      plot(flakes[i][0]-1, flakes[i][1], 0);
    }

    // Move down
    for (int i = 0; i < NUMFLAKES; i++) {
      flakes[i][1] = flakes[i][1] + 1;
      if (random(10) > 7) {
        flakes[i][0] = flakes[i][0] + (random(3) - 1);
      }
      if (flakes[i][1] > (Y_MAX + 1)) {
        flakes[i][1] = -1;
        flakes[i][0] = random(X_MAX + 1);
      }
    }
  }
}

unsigned char frame1[] ={
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000, 
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b01110000, 0b01110000, 0b01110000,
  0b10001000, 0b10001000, 0b10001000,
  0b00100010, 0b00100010, 0b00100010,
  0b00011100, 0b00011100, 0b00011100};

unsigned char frame2[] ={
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00011100, 0b00011100, 0b00011100,
  0b00100010, 0b00100010, 0b00100010,
  0b10001000, 0b10001000, 0b10001000,
  0b00000111, 0b00000111, 0b00000111};

unsigned char frame3[] ={
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000000, 0b00000000, 0b00000000,
  0b00000111, 0b00000111, 0b00000111,
  0b10001000, 0b10001000, 0b10001000,
  0b00100010, 0b00100010, 0b00100010,
  0b11000001, 0b11000001, 0b11000001};

void display_frame(unsigned char frame[]) {
  byte xbyte, xbit;
  byte y;
  for (y=0;y<16;y++){
    for (xbyte=0;xbyte<3;xbyte++){
      for (xbit=0; xbit<8; xbit++) {
        ht1632_plot ((xbyte<<3)+xbit,y, frame[(y+y+y+xbyte)] & (1<<xbit));
      }
    }
  }
}

const unsigned char PROGMEM frameA[384] ={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,
  1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
  0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,
  0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0};

const  unsigned char PROGMEM frameB[384] ={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,
  0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,
  1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
  0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1};

const  unsigned char PROGMEM frameC[384] ={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,
  1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,
  0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,
  1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1};

void display_pframe( const unsigned char frame[384]) {
  byte x=0;
  byte y=0;
  for (y=0;y<16;y++){
    for  (x=0;x<24;x++){
      ht1632_plot (x,y,pgm_read_byte_near(&frame[24*y+x]));
    }
  }
}  

void demo_pwaves()
{
  for (int i=0; i < (DEMOTIME/(DISPDELAY*3));  i++) {
    display_pframe(frameA);
    delay(DISPDELAY); // wait a bit.
    display_pframe(frameB);
    delay(DISPDELAY); // wait a bit.
    display_pframe(frameC);
    delay(DISPDELAY); // wait a bit.
  }
}
void demo_waves()
{
  for (int i=0; i < (DEMOTIME/(DISPDELAY*3));  i++) {
    display_frame(frame1);
    delay(DISPDELAY); // wait a bit.
    display_frame(frame2);
    delay(DISPDELAY); // wait a bit.
    display_frame(frame3);
    delay(DISPDELAY); // wait a bit.
  }
}

/***********************************************************************
 * traditional Arduino sketch functions: setup and loop.
 ***********************************************************************/

void setup ()  // flow chart from page 17 of datasheet
{
  ht1632_setup();
  randomSeed(analogRead(0));
}

void loop ()
{
  demo_pwaves();
  delay(LONGDELAY);

  demo_snowflakes();
  cls();
  demo_life();

  demo_chars();      // display chars from font table

  cls(); 
  demo_rowbrightness();

  demo_bouncyline();
  delay(LONGDELAY);

  bres_line(0,0, 16,16, 1);
  delay(LONGDELAY);

  demo_cross();
  delay(LONGDELAY);

  demo_scan();
  delay(LONGDELAY);

  demo_random_walk();

  delay(LONGDELAY);
}
