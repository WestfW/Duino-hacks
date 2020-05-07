
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
typedef int16_t intptr_t;

typedef uint64_t uintmax_t;
typedef int32_t int_farptr_t;



typedef uint32_t uint_farptr_t;

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;


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
const uint16_t 
              __attribute__((__progmem__)) 
                      port_to_mode_PGM[] = {
 0,
 (uint16_t) &
            (*(volatile uint8_t *)((0X01) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x04) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x07) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0A) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0D) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x10) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x13) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x101))
                ,
 0,
 (uint16_t) &
            (*(volatile uint8_t *)(0x104))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x107))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x10A))
                ,
};

const uint16_t 
              __attribute__((__progmem__)) 
                      port_to_output_PGM[] = {
 0,
 (uint16_t) &
            (*(volatile uint8_t *)((0X02) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x05) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x08) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0B) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0E) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x11) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x14) + 0x20))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x102))
                 ,
 0,
 (uint16_t) &
            (*(volatile uint8_t *)(0x105))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x108))
                 ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x10B))
                 ,
};

const uint16_t 
              __attribute__((__progmem__)) 
                      port_to_input_PGM[] = {
 0,
 (uint16_t) &
            (*(volatile uint8_t *)((0X00) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0X03) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x06) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x09) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0C) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x0F) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)((0x12) + 0x20))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x100))
                ,
 0,
 (uint16_t) &
            (*(volatile uint8_t *)(0x103))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x106))
                ,
 (uint16_t) &
            (*(volatile uint8_t *)(0x109))
                ,
};

const uint8_t 
             __attribute__((__progmem__)) 
                     digital_pin_to_port_PGM[] = {


 5 ,
 5 ,
 5 ,
 5 ,
 7 ,
 5 ,
 8 ,
 8 ,
 8 ,
 8 ,
 2 ,
 2 ,
 2 ,
 2 ,
 10 ,
 10 ,
 8 ,
 8 ,
 4 ,
 4 ,
 4 ,
 4 ,
 1 ,
 1 ,
 1 ,
 1 ,
 1 ,
 1 ,
 1 ,
 1 ,
 3 ,
 3 ,
 3 ,
 3 ,
 3 ,
 3 ,
 3 ,
 3 ,
 4 ,
 7 ,
 7 ,
 7 ,
 12 ,
 12 ,
 12 ,
 12 ,
 12 ,
 12 ,
 12 ,
 12 ,
 2 ,
 2 ,
 2 ,
 2 ,
 6 ,
 6 ,
 6 ,
 6 ,
 6 ,
 6 ,
 6 ,
 6 ,
 11 ,
 11 ,
 11 ,
 11 ,
 11 ,
 11 ,
 11 ,
 11 ,
};

const uint8_t 
             __attribute__((__progmem__)) 
                     digital_pin_to_bit_mask_PGM[] = {


 
(1 << (
0
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
7
)) 
         ,
 
(1 << (
0
)) 
         ,
 
(1 << (
1
)) 
         ,
 
(1 << (
2
)) 
         ,
 
(1 << (
3
)) 
         ,
 
(1 << (
4
)) 
         ,
 
(1 << (
5
)) 
         ,
 
(1 << (
6
)) 
         ,
 
(1 << (
7
)) 
         ,
};

const uint8_t 
             __attribute__((__progmem__)) 
                     digital_pin_to_timer_PGM[] = {


 0 ,
 0 ,
 10 ,
 11 ,
 2 ,
 9 ,
 12 ,
 13 ,
 14 ,
 8 ,
 7 ,
 3 ,
 4 ,
 1 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 18 ,
 17 ,
 16 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
 0 ,
};
typedef void (*voidFuncPtr)(void);


void pinMode(uint8_t pin, uint8_t mode)
{
 uint8_t bit = ( 
              (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
              digital_pin_to_bit_mask_PGM + (pin)
              )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
              );
 uint8_t port = ( 
               (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
               digital_pin_to_port_PGM + (pin)
               )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
               );
 volatile uint8_t *reg, *out;

 if (port == 0) return;


 reg = ( (volatile uint8_t *)( 
      (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
      port_to_mode_PGM + (port)
      )); uint16_t __result; __asm__ __volatile__ ( "lpm %A0, Z+" "\n\t" "lpm %B0, Z" "\n\t" : "=r" (__result), "=z" (__addr16) : "1" (__addr16) ); __result; }))
      ) );
 out = ( (volatile uint8_t *)( 
      (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
      port_to_output_PGM + (port)
      )); uint16_t __result; __asm__ __volatile__ ( "lpm %A0, Z+" "\n\t" "lpm %B0, Z" "\n\t" : "=r" (__result), "=z" (__addr16) : "1" (__addr16) ); __result; }))
      ) );

 if (mode == 0x0) {
  uint8_t oldSREG = 
                   (*(volatile uint8_t *)((0x3F) + 0x20))
                       ;
                
               __asm__ __volatile__ ("cli" ::: "memory")
                    ;
  *reg &= ~bit;
  *out &= ~bit;
  
 (*(volatile uint8_t *)((0x3F) + 0x20)) 
      = oldSREG;
 } else if (mode == 0x2) {
  uint8_t oldSREG = 
                   (*(volatile uint8_t *)((0x3F) + 0x20))
                       ;
                
               __asm__ __volatile__ ("cli" ::: "memory")
                    ;
  *reg &= ~bit;
  *out |= bit;
  
 (*(volatile uint8_t *)((0x3F) + 0x20)) 
      = oldSREG;
 } else {
  uint8_t oldSREG = 
                   (*(volatile uint8_t *)((0x3F) + 0x20))
                       ;
                
               __asm__ __volatile__ ("cli" ::: "memory")
                    ;
  *reg |= bit;
  
 (*(volatile uint8_t *)((0x3F) + 0x20)) 
      = oldSREG;
 }
}
static void turnOffPWM(uint8_t timer)
{

 switch (timer)
 {

  case 3: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
                 &= ~
                 (1 << (7))
                 ); break;


  case 4: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
                 &= ~
                 (1 << (5))
                 ); break;


  case 5: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x80)))))) 
                 &= ~
                 (1 << (3))
                 ); break;







  case 1: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
                 &= ~
                 (1 << (7))
                 ); break;



  case 2: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)((0x24) + 0x20)))))) 
                 &= ~
                 (1 << (5))
                 ); break;


  case 7: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB0)))))) 
                 &= ~
                 (1 << (7))
                 ); break;


  case 8: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xB0)))))) 
                 &= ~
                 (1 << (5))
                 ); break;



  case 9: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
                 &= ~
                 (1 << (7))
                 ); break;


  case 10: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
                 &= ~
                 (1 << (5))
                 ); break;


  case 11: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x90)))))) 
                 &= ~
                 (1 << (3))
                 ); break;



  case 12: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
                 &= ~
                 (1 << (7))
                 ); break;


  case 13: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
                 &= ~
                 (1 << (5))
                 ); break;


  case 14: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0xA0)))))) 
                 &= ~
                 (1 << (3))
                 ); break;






  case 16: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
                 &= ~
                 (1 << (7))
                 ); break;
  case 17: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
                 &= ~
                 (1 << (5))
                 ); break;
  case 18: (
                 (*(volatile uint8_t *)(((uint16_t) &((*(volatile uint8_t *)(0x120)))))) 
                 &= ~
                 (1 << (3))
                 ); break;

 }

}

void digitalWrite(uint8_t pin, uint8_t val)
{

 uint8_t timer = ( 
                (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
                digital_pin_to_timer_PGM + (pin)
                )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
                );
 uint8_t bit = ( 
              (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
              digital_pin_to_bit_mask_PGM + (pin)
              )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
              );
 uint8_t port = ( 
               (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
               digital_pin_to_port_PGM + (pin)
               )); uint8_t __result; __asm__ __volatile__ ( "lpm %0, Z" "\n\t" : "=r" (__result) : "z" (__addr16) ); __result; })) 
               );
 volatile uint8_t *out;

 if (port == 0) return;



 if (timer != 0) turnOffPWM(timer);

 out = ( (volatile uint8_t *)( 
      (__extension__({ uint16_t __addr16 = (uint16_t)((uint16_t)(
      port_to_output_PGM + (port)
      )); uint16_t __result; __asm__ __volatile__ ( "lpm %A0, Z+" "\n\t" "lpm %B0, Z" "\n\t" : "=r" (__result), "=z" (__addr16) : "1" (__addr16) ); __result; }))
      ) );

 uint8_t oldSREG = 
                  (*(volatile uint8_t *)((0x3F) + 0x20))
                      ;
 
__asm__ __volatile__ ("cli" ::: "memory")
     ;

 if (val == 0x0) {
  *out &= ~bit;
 } else {
  *out |= bit;
 }

 
(*(volatile uint8_t *)((0x3F) + 0x20)) 
     = oldSREG;

}
