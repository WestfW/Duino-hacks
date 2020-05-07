
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
typedef int16_t intptr_t;

typedef unsigned int size_t;
extern const void * memchr_P(const void *, int __val, size_t __len) __attribute__((__const__));

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
int analogRead(uint8_t);
void analogReference(uint8_t mode);
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
static const uint8_t SS = (53);
static const uint8_t MOSI = (51);
static const uint8_t MISO = (50);
static const uint8_t SCK = (52);




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


uint8_t analog_reference = 1;






void analogWrite(uint8_t pin, int val)
{





 pinMode(pin, 0x1);
 if (val == 0)
 {
  digitalWrite(pin, 0x0);
 }
 else if (val == 255)
 {
  digitalWrite(pin, 0x1);
 }
 else
 {
  switch(( 
        (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
        digital_pin_to_timer_PGM + (pin)
        )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
        ))
  {
   case 1:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
   |= 
   (1 << (7))
   );
    
   (*(volatile uint8_t *)((0X27) + 0x20)) 
         = val;
    break;



   case 2:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint8_t *)((0X28) + 0x20)) 
         = val;
    break;



   case 3:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
   |= 
   (1 << (7))
   );
    
   (*(volatile uint16_t *)(0x88)) 
         = val;
    break;



   case 4:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint16_t *)(0x8A)) 
         = val;
    break;



   case 5:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
   |= 
   (1 << (3))
   );
    
   (*(volatile uint16_t *)(0x8C)) 
         = val;
    break;
   case 7:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB0)))))) 
   |= 
   (1 << (7))
   );
    
   (*(volatile uint8_t *)(0xB3)) 
         = val;
    break;



   case 8:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB0)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint8_t *)(0xB4)) 
         = val;
    break;



   case 9:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
   |= 
   (1 << (7))
   );
    
   (*(volatile uint16_t *)(0x98)) 
         = val;
    break;



   case 10:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint16_t *)(0x9A)) 
         = val;
    break;



   case 11:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
   |= 
   (1 << (3))
   );
    
   (*(volatile uint16_t *)(0x9C)) 
         = val;
    break;



   case 12:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
   |= 
   (1 << (7))
   );

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
   &= ~
   (1 << (6))
   );

    
   (*(volatile uint16_t *)(0xA8)) 
         = val;
    break;



   case 13:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint16_t *)(0xAA)) 
         = val;
    break;



   case 14:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
   |= 
   (1 << (3))
   );
    
   (*(volatile uint16_t *)(0xAC)) 
         = val;
    break;
   case 16:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
   |= 
   (1 << (7))
   );
    
   (*(volatile uint16_t *)(0x128)) 
         = val;
    break;



   case 17:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
   |= 
   (1 << (5))
   );
    
   (*(volatile uint16_t *)(0x12A)) 
         = val;
    break;



   case 18:

    (
   (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
   |= 
   (1 << (3))
   );
    
   (*(volatile uint16_t *)(0x12C)) 
         = val;
    break;


   case 0:
   default:
    if (val < 128) {
     digitalWrite(pin, 0x0);
    } else {
     digitalWrite(pin, 0x1);
    }
  }
 }
}
