/*
 * Binary to BCD Conversion Test and measure Program.
 * See https://forum.arduino.cc/t/converting-binary-to-bcd/1017775
 * This only tests the conversion of 8bit numbers (0 <= n <= 255)
 */
#define var_t uint8_t

/*
 * Support Function
 * testValidity - Run the test method over the full range of
 *   input values, and check if the outputs are correct.
 */
void testValidity(const char *name, uint16_t (*f)(uint8_t),
                  uint16_t start = 0, uint16_t endv = 255) {
  uint16_t result;
  boolean fail = false;
  Serial.print(name);
  Serial.print(F(" Validity Test"));
  for (uint16_t i = start; i <= endv; i++) {
    result = f(i);
    if (/* any digits outside 0..9 ?) */
        ((result & 0xF) > 9 || (result & 0xF0) > 0x90 || (result & 0x0F00) > 0x900) ||
        /* summed value wrong ? */
        i != (result & 0xF) + ((result >> 4) & 0xF) * 10 +
        ((result >> 8) & 0xF) * 100 + ((result >> 12) & 0xF) * 1000) {
      if (!fail) Serial.println(); //prettify
      Serial.print(i); Serial.print(" ");
      Serial.print(result, HEX);
      Serial.println(F("  ****"));
      fail = true;
    }
  }
  if (fail) {
    Serial.println(F("\n***** Method had failures! *****"));
  } else {
    Serial.println(F(" - OK"));
  }
  Serial.println();
}

/*
 * Support Function
 * timeIt - Run the test method over the full range of
 *   input values, and report the total time in microseconds.
 */
void timeIt(const char *name, uint16_t (*f)(uint8_t),
            uint16_t start = 0, uint16_t endv = 255) {
  volatile uint16_t result;
  uint32_t oldtime, newtime;

  Serial.print(name);
  Serial.println(F(" Timing"));
  Serial.flush();  // don't let continuing output affect timing!
  oldtime = micros();
  for (uint16_t i = start; i < endv; ++i) {
    result = f(i);
  }
  newtime = micros() - oldtime;
  Serial.print(F("Execution time (us): "));
  Serial.print(newtime);
  Serial.print(F("  Average: "));
  Serial.print((float)newtime/256);
  Serial.println(F("\n-----------------------"));
}

/*
 * ------------------  Methods to test ----------------------------------
 */
extern "C" uint16_t DD8(uint8_t);   // assembler DoubleDabble Implementation (@westfw)
extern "C" uint16_t DD8s(uint8_t);  // assembler DoubleDabble (opt for size.) (@westfw)

/*
 * Original (and very general) DoubleDabble from the forum (@sherzaad)
 * https://forum.arduino.cc/t/converting-binary-to-bcd/1017775/24
 */

void Double_Dabble(var_t number, uint8_t *dg_arr, uint8_t arr_len) {
  //double dabble algorithm is used to convert binary numbers into binary-coded
  //decimal (BCD) notation. It is also known as the shift-and-add-3

  uint8_t MASK = 0x0F, ADD3 = 0x03, IS5 = 0x05;
  var_t p = 1;

  dg_arr[arr_len - 1] = number >> ((sizeof(var_t) * 8) - 2);

  for (int i = ((sizeof(var_t) * 8) - 3); i > 0; --i) {
    shift_left(dg_arr, 4, 1);
    dg_arr[arr_len - 1] |= ((number & (p << i)) ? 1 : 0);
    for (int j = 0; j < arr_len; ++j) {
      for (int k = 0; k < 8; k += 4) {
        if ((dg_arr[j] & (MASK << k)) >= (IS5 << k)) {
          dg_arr[j] += (ADD3 << k);
        }
      }
    }
  }
  shift_left(dg_arr, 4, 1);
  dg_arr[arr_len - 1] |= (number & p);
}

/*
 * Wrapper for DoubleDabble with proper function signature
 */
uint16_t DDgeneral(uint8_t n) {
  uint8_t dg[4] = {0};

  Double_Dabble(n, dg, 4);
  return (dg[2] << 8) + dg[3];
}

void shift_left(uint8_t *ar, int size, int shift)
{
  int carry = 0;                              // Clear the initial carry bit.
  while (shift--) {                           // For each bit to shift ...
    for (int i = size - 1; i >= 0; --i) { // For each element of the array from high to low ...
      int next = (ar[i] & 0x80) ? 1 : 0;          // ... if the low bit is set, set the carry bit.
      ar[i] = carry | (ar[i] << 1);       // Shift the element one bit left and addthe old carry.
      carry = next;                       // Remember the old carry for next time.
    }
  }
}

/*
 * Binary to BCD conversion using division.  (@sherzaad)
 * https://forum.arduino.cc/t/converting-binary-to-bcd/1017775/24
 */
uint8_t dec_to_packedBDC_div(var_t val, uint8_t *dg_arr, uint8_t arr_len) {
  var_t x = val;
  int bdc_bytes = arr_len, i = arr_len - 1, j = 0;

  dg_arr[0] = 0;

  while (x != 0) {
    if (j)dg_arr[i] |= ((x % 10) & 0x0F) << 4;
    else dg_arr[i] = (x % 10) & 0x0F;

    x /= 10;

    if (x != 0) {
      i -= j;
      j ^= 0x01;
    }

    if (i < 0) return 0;
  }

  return bdc_bytes - i;
}

// wrapper function
uint16_t BCD_byDivision(uint8_t n) {
  uint8_t dg[4] = {0};

  dec_to_packedBDC_div(n, dg, 4);
  return (dg[2] << 8) + dg[3];
}

/*
 * ----------------- Inline Double Dabble in C (@westfw)
 */

uint16_t DD8c(uint8_t in) {
  uint16_t n;
  uint16_t hundreds = 0;
  if (in > 199) {  // Do the hundreds digit separately
    hundreds = 0x200;  // BCD 200
    in -= 200;
  } else if (in > 99) {
    hundreds = 0x100;  // BCD 100
    in -= 100;
  } else {
    hundreds = 0;
  }
  // Now we have a value <= 99, and can do 16bit math
  n = in * 8;   // 3 shifts.

  if ((n & 0xF00) >= 0x500)  // ones digit
    n += 0x300;
  n <<= 1;   // 4th shift
  if ((n & 0xF00) >= 0x500)  // ones digit
    n += 0x300;
  n <<= 1;   // 5th shift
  if ((n & 0xF00) >= 0x500)  // ones digit
    n += 0x300;
  n <<= 1;   // 6th shift
  if ((n & 0xF00) >= 0x500)  // ones digit
    n += 0x300;
  if ((n >= 0x5000))         // tens digit
    n += 0x3000;
  n <<= 1;   // 7th shift
  if ((n & 0xF00) >= 0x500)  // ones digit
    n += 0x300;
  if ((n >= 0x5000))         // tens digit
    n += 0x3000;
  n <<= 1;   // 8th shift
  return (n >> 8) + hundreds;
}

/*
 * binary to BCD using division by repeated subtraction (@aarg)
 * https://forum.arduino.cc/t/converting-binary-to-bcd/1017775/43
 */
uint16_t dec_to_packedBDC_nodiv(uint8_t acc) {
  uint8_t hund = 0, tens = 0;

  // hundreds digit
  while (acc >= 100)
  {
    hund++;
    acc -= 100;
  }
  // tens digit
  while (acc >= 10)
  {
    tens += 0x10;
    acc -= 10;
  }
  // units digit is left
  return ((hund << 8) + tens + acc);
}

/*
 *
 */

unsigned long inpBin = 0b000000000000000010100010;//162 = 0xA2
byte colOne = 0, colTen = 0, colHun = 0;

uint16_t golam(uint8_t n) {
  inpBin = n;
  for (int i = 0; i < 8; i++)
  {
    inpBin = inpBin << 1;
    pullCol();
    if (colOne >= 5)
    {
      if (i != 7)
      {
        colOne += 3;
      }
    }
    if (colTen >= 5)
    {
      if (i != 7)
      {
        colTen += 3;
      }
    }
    if (colHun >= 5)
    {
      if (i != 7 )
      {
        colHun += 3;
      }
    }
    pushCol();
  }
  return (colHun << 8) | (colTen << 4) | (colOne);
}

void pullCol()
{
  for (int i = 0, j = 8; i < 4; i++, j++)
  {
    bitWrite(colOne, i, bitRead(inpBin, j));
  }

  for (int i = 0, j = 12; i < 4; i++, j++)
  {
    bitWrite(colTen, i, bitRead(inpBin, j));
  }

  for (int i = 0, j = 16; i < 4; i++, j++)
  {
    bitWrite(colHun, i, bitRead(inpBin, j));
  }
}

void pushCol()
{
  for (int i = 8, j = 0; i < 12; i++, j++)
  {
    bitWrite(inpBin, i, bitRead(colOne, j));
  }
  for (int i = 12, j = 0; i < 16; i++, j++)
  {
    bitWrite(inpBin, i, bitRead(colTen, j));
  }
  for (int i = 16, j = 0; i < 20; i++, j++)
  {
    bitWrite(inpBin, i, bitRead(colHun, j));
  }
}

/*
 * Arduino setup code
 */
void setup() __attribute__((noinline));  // for easier visualization
void setup() {
  var_t number = 162;
  uint8_t dg[4];
  uint32_t oldtime, newtime;

  Serial.begin(115200);
  testValidity(" (@golam)", golam);
  testValidity("Double Dabble (Generalized) (@sherzaad)", DDgeneral);
  testValidity("BCD_byDivision (@sherzaad)", BCD_byDivision);
  testValidity("DD8 ASM (fast) optimized (@westfw)", DD8);
  testValidity("DD8s ASM (small) optimized (@westfw)", DD8s);
  testValidity("DD8c C optimized (@westfw)", DD8c);
  testValidity("BCD by repeated subtraction (@aarg)", dec_to_packedBDC_nodiv);
  Serial.println("\nTimings (loop over all 256 input values)\n");
  timeIt(" (@golam)", golam);
  timeIt("Double Dabble (Generalized) (@sherzaad)", DDgeneral);
  timeIt("BCD_byDivision (@sherzaad)", BCD_byDivision);
  timeIt("DD8 ASM (fast) optimized (@westfw)", DD8);
  timeIt("DD8s ASM (small) optimized (@westfw)", DD8s);
  timeIt("DD8c C optimized (@westfw)", DD8c);
  timeIt("BCD by repeated subtraction (@aarg)", dec_to_packedBDC_nodiv);
}


void loop() {
  // empty loop; everything is in setup()
}
