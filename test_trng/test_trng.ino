
/*
   See if we can get the SAMD41 True Random Number Gneerator (TRNG) working
*/
#if !defined(TRNG)
#error Chip has no TRNG peripheral
#else
#if !( defined(__SAMD51__))
#error don't understand TRNG on this chip/board
#endif
#endif
void setup() {
  Serial.begin(1);
  while (!Serial)
    ;
  Serial.println("TRNG test\n");
  MCLK->APBCMASK.bit.TRNG_ = 1;  // enable clock  (APBC clock is already enabled)
  TRNG->CTRLA.bit.ENABLE = 1;    // enable the TRNG
}

uint32_t get_trng() {
  while ((TRNG->INTFLAG.reg & TRNG_INTFLAG_MASK) == 0)
    ;  // Busy loop waiting for next RN to be created
  return (TRNG->DATA.reg);   // return the new number.
}

void loop() {
  for (int i = 0; i < 50; i++) {
    Serial.println(get_trng());
  }
  while (Serial.read() < 0)
    ;
}
