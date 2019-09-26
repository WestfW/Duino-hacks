boolean NonConstantUsed( void ) __attribute__ (( error("") ));
void LTO_Not_Enabled(void) __attribute__ (( error("") ));

#define digitalWriteFast(pin, val)          \
    if (__builtin_constant_p(pin) /*&& __builtin_constant_p(val)*/) {_dwfast(pin, val);} else {NonConstantUsed();}

static inline  __attribute__((always_inline)) void _dwfast(int pin, int val) {
    if (val) {
	digitalPinToPort(pin)->PIO_SODR = digitalPinToBitMask(pin);
    } else {
	digitalPinToPort(pin)->PIO_CODR = digitalPinToBitMask(pin);
    }
}
