/*
 * Functions for timing relatively short code sequencing.
 * When trying to benchmark certain functions (like digitalWrite()), it's
 * common to loop through a code sequence "many" times so that relatively
 * slow clocks (millis()) can be used to calculate the runtime.
 * However, with various compiler and link-time optimizations, this can
 * result in inaccuracies, because various calculations may have been
 * moved out of the loop.
 * This code attempts to address this by providing functions that time
 * code sequences in units of the native CPU clock cycle, that therefore
 * work ok for timing single instances of a piece of code.
 * On ARM we will used the CYCCNT in the DWT unit, if present, or Systick
 * otherwise (this means that you won't be able to time code that takes more
 * than 1ms, sinc Systick is also usually used to generate the 1ms clock tick.)
 * On AVR, we'll re-purpose one of the timers to run at maximum speed.
 *
 *  2023 by Bill Westfield (WestfW)
 */
#define debug(a)

class cycletimer {
 private:
  uint32_t _starttime;
  uint32_t _offset;
  uint32_t cycles_bare() {
    uint32_t endtime = DWT->CYCCNT;
    return endtime - starttime;
  }
  void calibrate(void) {
    start();
    _offset = cycles_bare() - starttime;
  }
    
 public:
  void init() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    calibrate();
  }
  void start() {
    starttime = DWT->CYCCNT;
  }
  void end() {
/*
 *  If out chip has the Debug Watch & Trace unit, we can use its full 32bit
 *  Cycle counter (DWT->CYCCNT) and worry less about wrapping due
 *  to Systick->load   (in theory.  Not yet implemented.)
 */
#ifdef DWT
void delayus_dwt(uint32_t us)
{
  uint32_t start, elapsed;
  uint32_t count;

  if (us == 0)
    return;

  count = us * (VARIANT_MCK / 1000000) - 20;
  start = DWT->CYCCNT;  //CYCCNT is 32bits, takes 37s or so to wrap.
  while (1) {
    elapsed = DWT->CYCCNT - start;
    if (elapsed >= count)
      return;
  }
}
#endif

/*
 * You can change the definition of GET_SYSTICK if your chip doesn't have
 * the standard CMSIS definitions.
 8/
#define GET_SYSTICK SysTick->VAL;

/*
   Delay for N cycles, using sysTick Timer.
   Systick must be running.  count should be significantly less than the LOAD value,
   so this is probably good for 100 cycles < delay < 100us (or less) with a 1ms systick interrupt.
   (likewise, obviously not accurate to 1 cycle.
     Probably accurate to within ~20 cycles, except for interrupts.)

     (remember SYSTICK counts DOWN!)
     eeeeeE--------SeeeeeeeeeeeeeeeeeeR  (no wrap)
     --SeeeeeeeeeeeeeeeeeeeeeeeeeE----R  (wrap - end time E > start time S)
*/
void sysTickDelay (uint32_t count)
{
  uint32_t reload = SysTick->LOAD;
  int32_t start, end, now;

  if (us == 0)
    return;

  debug("get start >");
  start = GET_SYSTICK;
  end = start - count;
  if (end > 0) {
    while (1) {
      now = GET_SYSTICK;
      if ((now <= end) || (now > start))
        return;
    }
  } else {
    end += reload;  // wrap the end time
    while (1) {
      now = GET_SYSTICK;
      if ((now <= end) && (now > start))
        return;
    }
  }
}

void sysTickDelay2 (uint32_t count)
{
  int32_t start, elapsed, reload;

  if (us == 0)
    return;

  start = GET_SYSTICK;
  reload = SysTick->LOAD;
  while (1) {
    elapsed = start - SysTick->VAL;
    if (elapsed < 0)
      elapsed += reload;  // Handle wrap.
    if (elapsed >= count)
      return;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
}

char s[80];

void loop() {
  uint32_t starttime, endtime;
#if 0
  while (1) {
    digitalWrite(2, 1);
    digitalWrite(2, 0);
  }
#endif
#if 0
  while (1) {
    PIOB->PIO_SODR = 1 << 25;
    // asm volatile ("isb\n isb\n isb\n isb");
    sysTickDelay(84);
    PIOB->PIO_CODR = 1 << 25;
    sysTickDelay(84);
  }
#endif
  for (uint32_t i = 5; i <= 100; i += 5) {
    uint32_t thisdelay = 84 * i;
    sprintf(s, "Target delay of %d us\n", i);
    Serial.print(s);
    thisdelay -= 2 * 84;
    while (1) {
      digitalWrite(2, 1);
      sysTickDelay2(thisdelay);
      digitalWrite(2, 0);
      sysTickDelay2(thisdelay);
      if (Serial.read() > 0) break;
    }
#if 0
    delay(1000);
    for (int j = 0; j < 20; j++) {

      starttime = micros();
      digitalWrite(2, 1);
      sysTickDelay(thisdelay);
      digitalWrite(2, 0);
      endtime = micros();
      sysTickDelay(thisdelay);
      //      sprintf(s, "  SysTick Delay %d us, start %x, end %x\n", endtime - starttime, starttime, endtime);
      //    Serial.print(s);
      //   delay (2);
    }
#endif
  }
}


#ifdef CMCC
inline void delayus_cmcc(uint32_t us)
{
  uint32_t count;

  if (us == 0)
    return;

   // Enable Cache Monitor; defaults to Cycle Counting
  CMCC->MEN.reg = CMCC_MEN_MENABLE;
  CMCC->MCTRL.reg = CMCC_MCTRL_SWRST; // reset counter.
  count = us * (VARIANT_MCK / 1000000) - 20;
  
  while (CMCC->MSR.reg < count) {
    ; // spin
  }
}
#endif
