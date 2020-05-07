

typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
typedef unsigned int size_t;

extern "C"{


void yield(void);
typedef unsigned int word;



typedef bool boolean;
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
}
uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);



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
void setup(void);
void loop(void);
void setup(void) {
  analogWrite(6, 100);
  analogWrite(5, 200);
}

void loop(void) {}
