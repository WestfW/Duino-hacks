/*
 * Functions that delay a specific number of clock cycles, in spite of
 *  non-deterministic instruction timing (due to cache, "flash acceleration",
 *  and so on.)  These use the SYSTICK which has to be running.
 *
 *  2018 by Bill Westfield (WestfW)
 */
#define debug(a)

/*
 * There are potentially several counters in an ARM that count main CPU clock cycles.

 * 1) There is the SysTick Timer, which is limited to 24 bits, counts
 *    down, and is usually further limited by its use as a System Tick
 *    Timer (resets after 1ms)
 *
 * 2) The Data Watchpoint and Trace unit (DWT) is an option of ARMv7m
 *    architecture chips (CM3, CM4, etc) - if enabled, it keeps a 32
 *    cycle up-counter.
 *
 * 3) If a chip has a cache controller, that may includes monitoring
 *    hardware that may keep a cycle count.  (This is vendor
 *    specific. For example, the SAMD51 has a CMCC->MSR.EVENT_COUNT
 *    that can count cycles, data cache hits, or code hits.
 *    This counter does not wrap!
 */


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
