
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef unsigned int size_t;

void yield(void);
typedef unsigned int word;



typedef 
       _Bool 
            boolean;
typedef uint8_t byte;

void init(void);
void initVariant(void);

int atexit(void (*func)()) __attribute__((weak));

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);
void analogWrite(uint8_t, int);

void setup(void);
void loop(void);
extern const uint16_t 
                     __attribute__((__progmem__)) 
                             port_to_mode_PGM[];
extern const uint16_t 
                     __attribute__((__progmem__)) 
                             port_to_input_PGM[];
extern const uint16_t 
                     __attribute__((__progmem__)) 
                             port_to_output_PGM[];

extern const uint8_t 
                    __attribute__((__progmem__)) 
                            digital_pin_to_port_PGM[];

extern const uint8_t 
                    __attribute__((__progmem__)) 
                            digital_pin_to_bit_mask_PGM[];
extern const uint8_t 
                    __attribute__((__progmem__)) 
                            digital_pin_to_timer_PGM[];

static const uint8_t SDA = (20);
static const uint8_t SCL = (21);
static const uint8_t A0 = (54);
static const uint8_t A1 = (55);
static const uint8_t A2 = (56);
static const uint8_t A3 = (57);
static const uint8_t A4 = (58);
static const uint8_t A5 = (59);
static const uint8_t A6 = (60);
static const uint8_t A7 = (61);
static const uint8_t A8 = (62);
static const uint8_t A9 = (63);
static const uint8_t A10 = (64);
static const uint8_t A11 = (65);
static const uint8_t A12 = (66);
static const uint8_t A13 = (67);
static const uint8_t A14 = (68);
static const uint8_t A15 = (69);
uint32_t countPulseASM(volatile uint8_t *port, uint8_t bit, uint8_t stateMask, unsigned long maxloops);
typedef void (*voidFuncPtr)(void);
volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;





void __vector_23 (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_23 (void)


{


 unsigned long m = timer0_millis;
 unsigned char f = timer0_fract;

 m += ((( (64 * 256) / ( 16000000L / 1000000L ) )) / 1000);
 f += (((( (64 * 256) / ( 16000000L / 1000000L ) )) % 1000) >> 3);
 if (f >= (1000 >> 3)) {
  f -= (1000 >> 3);
  m += 1;
 }

 timer0_fract = f;
 timer0_millis = m;
 timer0_overflow_count++;
}

void init()
{


 
__asm__ __volatile__ ("sei" ::: "memory")
     ;





 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
|= 
(1 << (1))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x25) + 0x20)))))) 
|= 
(1 << (1))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x25) + 0x20)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x6E)))))) 
|= 
(1 << (0))
);
 
(*(volatile uint8_t *)(0x81)) 
       = 0;


 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x81)))))) 
|= 
(1 << (1))
);

 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x81)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
|= 
(1 << (0))
);






 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB1)))))) 
|= 
(1 << (2))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB0)))))) 
|= 
(1 << (0))
);





 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x91)))))) 
|= 
(1 << (1))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x91)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA1)))))) 
|= 
(1 << (1))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA1)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
|= 
(1 << (0))
);




 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x121)))))) 
|= 
(1 << (1))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x121)))))) 
|= 
(1 << (0))
);
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
|= 
(1 << (0))
);





  (
 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x7A)))))) 
 |= 
 (1 << (2))
 );
  (
 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x7A)))))) 
 |= 
 (1 << (1))
 );
  (
 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x7A)))))) 
 |= 
 (1 << (0))
 );
 (
(*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x7A)))))) 
|= 
(1 << (7))
);
 
(*(volatile uint8_t *)(0XC1)) 
       = 0;

}
