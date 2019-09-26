boolean NonConstantUsed( void ) __attribute__ (( error("") ));
void LTO_Not_Enabled(void) __attribute__ (( error("") ));

#define digitalWriteFast(pin, val)          \
    if (__builtin_constant_p(pin) /*&& __builtin_constant_p(val)*/) {_dwfast(pin, val);} else {NonConstantUsed();}

static inline  __attribute__((always_inline)) void _dwfast(int pin, int val) {
  PortGroup *pPort = digitalPinToPort(pin);

  // Handle the case the pin isn't usable as PIO
  if ( pPort == NULL ) {
    return ;
  }

  uint32_t pinMask = (1ul << (g_APinDescription[pin].ulPin));

  if (val) {
      pPort->OUTSET.reg = pinMask;
  } else {
      pPort->OUTCLR.reg = pinMask;
  }
}
