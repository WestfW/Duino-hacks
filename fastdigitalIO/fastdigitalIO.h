/*
 * Improved fast digital IO.
 * Copyright November 2018 by Bill Westfield.   BSD license.
 *
 * Various implementations have been written so that Arduino programs
 * can take advantage of the single-instruction AVR bitset/bitclear
 * operations.  They typically rely on complex and device-dependent
 * replacements for the progmem tables that map Arduino "pin numbers"
 * to AVR ports and bit masks, making them very fragile.
 *
 * With the use of link-time-optimiztion, and by making use of a couple
 * of gcc "features", we can do much better:
 *
 * Feature 1) neither gcc nor the early link phases are "aware" of PROGMEM
 *    - symbols in PROGMEM can be accessed like normal variables (without
 *    pgm_read_xxx()), and the compiler will retrieve the proper constants.
 *
 * Feature 2) one of the constant-based optimizations that gcc will do is
 *    array access.  If array and i are both constants, array[i] will be
 *    done at compile time.
 *
 * Feature 3) with LTO, the compiler can do that constant optimization
 *    on arrays in PROGMEM that are actually defined in other modules.
 *
 * Used together, this means that a digitalWriteFast() inline function
 * used in the main user sketch can access the same PROGMEM tables that
 * the normal digitalWrite() function uses to do the port/mask
 * translation.  If all the values involved are constants, code that
 * looks very like the normal digitalWrite() function will end up
 * producing the single bitset/bitclear instructions (assuming that the
 * ports are withing range.)
 *
 * We can detect whether LTO is enabled by checking an external global
 * variable for a "known impossible" value.  Under LTO, the test will
 * always fail (at compile time - no actual code produced!)  If LTO is
 * not enabled, the compiler will encounter a function call with the
 * "error" attribute, which will result in the compilation aborting.
 */

boolean NonConstantUsed( void ) __attribute__ (( error("") ));
void LTO_Not_Enabled(void) __attribute__ (( error("") ));

#define digitalWriteFast(pin, val)                                      \
    if (__builtin_constant_p(pin) /*&& __builtin_constant_p(val)*/) {_dwfast(pin, val);} else {NonConstantUsed();}

#if defined(VPORTB)
static inline  __attribute__((always_inline)) void _dwfast(int pin, int val) {
/*
 * Mega-0, Tiny-1 style IOPORTs
 * (assumes VPORTs exist starting at 0 for each PORT structure)
 */
    uint8_t mask = 1<<digital_pin_to_bit_position[pin];
    uint8_t port = digital_pin_to_port[pin];
    VPORT_t *vport;
    if (digital_pin_to_bit_position[0] == 0x99) {  // never true.
        LTO_Not_Enabled();
    }
    if (port > 0x1F) {
        /*
         * "FASTER" port logic is low bits of PORTB struct address
         * spaced every 32 bytes.  PORTA is still 0.
         */
        vport = (VPORT_t *) ((port / 8));
    } else {
        /*
         * Old style port logic is a small integer 0 for PORTA, 1 for PORTB
         * etc. 
         */
        vport = (VPORT_t *)(port * 4);
    }
    if (val) {
        vport->OUT |= mask;
    } else {
        vport->OUT &= ~mask;
    }
}

#else

static inline __attribute__((always_inline)) void _dwfast(int pin, int val) {
    uint8_t mask = digital_pin_to_bit_mask_PGM[pin];
    uint8_t port = digital_pin_to_port_PGM[pin];
    volatile uint8_t *outport = (volatile uint8_t*)port_to_output_PGM[port];
    if (digital_pin_to_bit_mask_PGM[0] == 0x99)  // Never true
        LTO_Not_Enabled();
    if (val) {
        *outport |= mask;
    } else {
        *outport &= ~mask;
    }
}

#endif

/* ----------------------------------------------------------------------*/

#define digitalReadFast(pin)                                            \
    (__builtin_constant_p(pin) ? (!!_drfast(pin)) : NonConstantUsed())

#if defined(VPORTB)
static inline  __attribute__((always_inline)) uint8_t _drfast(uint8_t pin) {
/*
 * Mega-0, Tiny-1 style IOPORTs
 * (assumes VPORTs exist starting at 0 for each PORT structure)
 */
    uint8_t mask = 1<<digital_pin_to_bit_position[pin];
    uint8_t port = digital_pin_to_port[pin];
    VPORT_t *vport;
    if (digital_pin_to_bit_position[0] == 0x99) {  // never true.
        LTO_Not_Enabled();
    }
    if (port > 0x1F) {
        /*
         * "FASTER" port logic is low bits of PORTB struct address
         *  spaced every 32 bytes.  PORTA is still 0.
         */
        vport = (VPORT_t *) ((port / 8));
    } else {
        /*
         * Normal style port logic is a small integer 0 for PORTA,
         *  1 for PORTB, etc. 
         */
        vport = (VPORT_t *)(port * 4);
    }
    return vport->IN & mask;
}

#else
/*
 * Standard AVRs with ports "possibly" in range of sbic/in
 */
static inline __attribute__((always_inline)) uint8_t _drfast(uint8_t pin) {
    uint8_t mask = digital_pin_to_bit_mask_PGM[pin];
    uint8_t port = digital_pin_to_port_PGM[pin];
    volatile uint8_t *inport = (volatile uint8_t*)port_to_input_PGM[port];
    if (digital_pin_to_bit_mask_PGM[0] == 0x99)  // Never true
        LTO_Not_Enabled();
    return (*inport & mask);
}
#endif
