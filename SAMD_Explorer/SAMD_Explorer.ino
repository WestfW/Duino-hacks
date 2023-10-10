/*
   Written 2020 by Bill Westfield (WestfW)
   Released to the public domain.
*/
#include "simpleParser.h"
#include <stdio.h>

#define xstr(s) str(s)
#define str(s) #s
#define SHOWSYM(a) do {sprintf(spbuffer, PSTR("Value of '" #a "' is %ld\n"), (long)a); Serial.print(spbuffer); } while (0)
#define SHOWSYMS(a) do {sprintf(spbuffer, PSTR("Value of '" #a "' is '" a "'\n")); Serial.print(spbuffer); } while (0)
#define mySerial Serial

struct simreg {
  volatile uint8_t a, b, c, d;
};
#define SIMPORTS ((simreg *)(0x30000000))
#define SIMPORTA (SIMPORTS->a)
#define SIMPORTB (SIMPORTS->b)

simpleParser<> ttycli(Serial);

char spbuffer[132];
void setup() {
  Serial.begin(115200);
  //  SIMPORTB = 0x23;
  //  SIMPORTA = 0x46;
  while (!Serial)
    delay(500);

#if 0
  for (uint32_t i = 0; i < NUM_ANALOG_INPUTS; i++) {
    sprintf(spbuffer, "Analog%ld = %ld\n", i, analogRead(i));
    Serial.print(spbuffer);
  }
#endif
  Serial.println("\n--------------------\nType \"help\" for help\n");
  Serial.print(F("SAMD Explorer> "));
  Serial1.begin(9600);
  pinMode(PIN_WIRE_SDA, INPUT);
  attachInterrupt(PIN_WIRE_SDA, myCallback, CHANGE);
}

volatile boolean sawInterrupt = 0;

void myCallback() {
  sawInterrupt = true;
}

void loop() {
  backgroundMonitor();
}


extern "C" uint32_t *exception_table;
extern "C" void Dummy_Handler();

const char * vecnames[] = {
  "Initial SP",
  "Reset",
  "NMI",
  "HardFault",
  "MemManage",
  "BusFault",
  "UsageFault",
  "xxx",  "xxx",  "xxx",  "xxx",  // "reserved"
  "SVCall",
  "DebugMonitor",
  "xxx",
  "PendSV",
  "SysTick"
};

void backgroundMonitor() {
  int8_t cmd;
  int n, end, start;
  if (ttycli.getLine()) {  // read a line of text
#if 0
    if (ttycli.eol()) {
      ttycli.reset();
      return;
    }
#endif
    static const char *cmd_strings =
      PSTR("board ""bytes ""hwords "   "words " "pwm " "interrupts " "sercom "   "pins "   "help "    "? ");
    enum {
      CMD_BOARD, CMD_BYTE, CMD_HWORD, CMD_WORD, CMD_PWM, CMD_INT,    CMD_SERCOM, CMD_PINS, CMD_HELP, CMD_HELP2  // make sure this matches the string
    };

    cmd = ttycli.keyword(cmd_strings); // look for a command.

    switch (cmd) {
      case CMD_BOARD:
        SHOWSYMS(USB_MANUFACTURER);
        SHOWSYMS(USB_PRODUCT);
#ifdef __SKETCH_NAME__
        SHOWSYMS(__SKETCH_NAME__);
#endif
        SHOWSYMS(__VERSION__);
        SHOWSYMS(_NEWLIB_VERSION);
        SHOWSYM(F_CPU);
        SHOWSYM(sizeof(int));
        SHOWSYM(sizeof(double));
        SHOWSYM(sizeof (float));
        SHOWSYM(NUM_DIGITAL_PINS);
        SHOWSYM(NUM_ANALOG_INPUTS);
        SHOWSYM(PINS_COUNT);
        break;
      case CMD_INT:
        {
          uint32_t *vecs = (uint32_t *)(&exception_table);
          if (__VTOR_PRESENT) {
            vecs = (uint32_t *)SCB->VTOR;
          }
          Serial.print("\nException Table @0x");
          Serial.println((uint32_t)vecs, HEX); Serial.println("---------------");
          for (int i = 0; i < 16; i++) {
            uint32_t thisVec = vecs[i] & ~1;
            if (thisVec != 0 && thisVec != (~1 & (uint32_t) &Dummy_Handler)) {
              Serial.print(vecnames[i]);
              Serial.print(" = 0x");
              Serial.println(thisVec, HEX);
            }
          }
          for (int i = 0; i < PERIPH_COUNT_IRQn; i++) {
            uint32_t thisVec = vecs[i + 16] & ~1;
            if (thisVec != 0 && thisVec != (~1 & (uint32_t) &Dummy_Handler)) {
              Serial.print("Vector ");
              Serial.print(i);
              Serial.print(" = 0x");
              Serial.println(thisVec, HEX);
            }
          }
          Serial.println();
          break;
        }

      case CMD_PWM:
        end = NUM_DIGITAL_PINS;
        start = 0;
        n = ttycli.number();
        if (n > 0) {
          start = n;
          end = n + 1;
        }
        for (int i = start; i < end; i++) {
          if (g_APinDescription[i].ulPWMChannel != NOT_ON_PWM) {
            sprintf(spbuffer, "Pin #%2d has PWM Channel 0x%03x\n", i, g_APinDescription[i].ulPWMChannel);
            Serial.print(spbuffer);
          }
        }
        Serial.println();
        break;

      case CMD_SERCOM:
        for (unsigned int i = 0; i < NUM_DIGITAL_PINS; i++) {
          if (g_APinDescription[i].ulPinType == PIO_SERCOM) {
            sprintf(spbuffer, "Pin #%2d has SERCOM\n", i);
            Serial.print(spbuffer);
          }
          if (g_APinDescription[i].ulPinType == PIO_SERCOM_ALT) {
            sprintf(spbuffer, "Pin #%2d has SERCOM_ALT\n", i);
            Serial.print(spbuffer);
          }
        }
        break;

      case CMD_PINS:
        {
          int start = 0, end = PINS_COUNT;
          if ((n = ttycli.number()) >= 0) {
            start = n;
            end = start + 1;
          }
          #if 0 //  grr.  Doesn't work.
          // In fact, there's no current way to check the actual number of pins
          //  defined in the g_APinDescription array (which is frequently many more
          //  than PINS_COUNT.  Sigh.
          if (start > sizeof(g_APinDescription)/sizeof(*g_APinDescription)) {
            mySerial.println("Non-existant pin # (exceeds g_APinDescription bounds)\n");
          }
          #endif
          for (int pin = start; pin < end; pin++) {
            if (print_pinno(pin)) {  // Does pin exist?

              int bitno = g_APinDescription[pin].ulPin;
              int portno = g_APinDescription[pin].ulPort;
              PortGroup *port = digitalPinToPort(pin);
              int pmuxval;

              if (port->PINCFG[bitno].bit.PMUXEN) {  // Is pinmux enabled?
                // If pinmux is enabled, further report it's value
                if (bitno & 1) {
                  // Odd pin numbers
                  pmuxval = port->PMUX[bitno / 2].bit.PMUXO;
                } else {
                  pmuxval = port->PMUX[bitno / 2].bit.PMUXE;
                }
                print_pinmux(portno, bitno, pmuxval);
              } else {
                // Pinmux not enabled - port is GPIO
                mySerial.print("GPIO ");
                if (port->DIR.reg & (1 << bitno)) { // Decode pinmode (direction.)
                  mySerial.print("O");
                } else {
                  mySerial.print("I");
                }
              } // Pinmux or GPIO
            } // Pin existance
            mySerial.println();
          } // "pin" for loop
        }
        break;

      case CMD_HELP2:
      case CMD_HELP:
        Serial.println(F("Enter a type of info.  One of:"));
        Serial.println(reinterpret_cast<const __FlashStringHelper *>(cmd_strings));
        break;

      case CMD_BYTE:
        {
          uint8_t *p;
          n = ttycli.number();
          p = (uint8_t *) n;
          sprintf(spbuffer, "\n0x%08lx: ", (uint32_t)n);
          Serial.print(spbuffer);
          if (n >= 0) {
            for (int i = 0; i < 32; i++) {
              sprintf(spbuffer, "%02x ", p[i]);
              Serial.print(spbuffer);
            }
            Serial.print("  ");
            for (int i = 0; i < 32; i++) {
              if (p[i] < 32)
                Serial.print(".");
              else
                Serial.write(p[i]);
            }
            Serial.println();
          }
        }
        break;

      case CMD_HWORD:
        {
          uint16_t *p;
          n = ttycli.number();
          p = (uint16_t *) n;
          sprintf(spbuffer, "\n0x%08lx: ", (uint32_t)n);
          Serial.print(spbuffer);
          if (n >= 0) {
            for (int i = 0; i < 16; i++) {
              sprintf(spbuffer, "%04x ", p[i]);
              Serial.print(spbuffer);
            }
            Serial.println();
          }
        }
        break;

      case CMD_WORD:
        {
          uint32_t *p;
          n = ttycli.number();
          p = (uint32_t *) n;
          sprintf(spbuffer, "\n0x%08lx: ", (uint32_t)n);
          Serial.print(spbuffer);
          if (n >= 0) {
            for (int i = 0; i < 8; i++) {
              sprintf(spbuffer, "%08lx ", p[i]);
              Serial.print(spbuffer);
            }
            Serial.println();
          }
        }
        break;

      case PARSER_AMB:
        Serial.println(F("Ambigious"));
        break;

      case PARSER_EOL:
        break;

      default:
        Serial.println(F("Invalid command"));
        break;
    }

    Serial.print(F("\nSAMD Explorer> "));
    ttycli.reset();
  }
}

/*
   Pin multiplexer value names (PMUXn.PMUXE/O)
*/
const char *pinmux_names[] = {
#ifdef __SAMD51__
  "EIC",   // "A": External Interrupts
  "ADC",   // "B": REF, ADC, AC, PTC, DAC
  "SERCOM",   /* "C": SerCom */
  "SERCOMALT",  /* "D": Alternate SERCOM */
  "TC",         /* "E": TC Timer */
  "TCC",        /* "F": TCC Timer*/
  "TCC/PDEC",   /* "G": TCC/PDEC */
  "COM",         /* "H": QSPI, USB, DEBUG */
  "SDHC",       /* "I": SDHC, CAN0 */
  "I2S",        /* "J": i2s */
  "PCC",        /* "K": parallel com */
  "GMAC",       /*  DMA */
  "GCLK/AC",    /*  M: gclk, comparator */
  "CCL",        /*  N: Configurable logic */
#else
  "EIC",   // "A": External Interrupts
  "ADC",   // "B": REF, ADC, AC, PTC, DAC
  "SERCOM",   /* "C": SerCom */
  "SERCOMALT",  /* "D": Alternate SERCOM */
  "TC/TCC",     /* "E": TC or TCC Timer */
  "TCC",        /* "F": TCC Timer*/
  "COM",        /* "G": Other Comm - USB, I2S */
  "AC/GCLK"     /* "H": Analog Comparator or GCLKs */
#endif
};

extern "C" long _write_r ( struct _reent *ptr, int fd, const void *buf, size_t cnt )
{
  if (fd == 1)
    Serial.write((char *)buf, cnt);
  else {
    Serial.print("_write_r with fd=");
    Serial.println(fd);
  }
  return 0;
}

const char *portNames[] = {"PortA", "PortB", "PortC", "PortD"};

boolean print_pinno(int i)
{
  int pintype = g_APinDescription[i].ulPinType;
  if (pintype == PIO_NOT_A_PIN) {
    return false;
  }
  if (i < 10)
    mySerial.print(" ");
  mySerial.print(i);

  if (pintype == PIO_ANALOG) {  // try to detect and renumber Analog pins.
    int analogPin = i - PIN_A0;
#if defined(PIN_A8)
    if (analogPin < 0) {
      analogPin = (i - PIN_A8) + 8;
    }
#endif
    if (analogPin < 10)
      mySerial.print(" ");
      mySerial.print(" (A");
    mySerial.print(analogPin);
    mySerial.print(") ");
  } else {
    mySerial.print("       ");
  }
  mySerial.print("(");
  mySerial.print(portNames[g_APinDescription[i].ulPort]);
  int pinNo = g_APinDescription[i].ulPin;
  if (pinNo < 10) mySerial.print("0");
  mySerial.print(pinNo);
  mySerial.print(") ");
  return true;
}

extern const char * const * const pmux_tables[];
void print_pinmux(int port, int pin, int pmuxval)
{
  const char * const * this_pmux = pmux_tables[pmuxval];
  //  sprintf(spbuffer, "print_pinmux port=%d, pin=%d, muxval=%d", port, pin, pmuxval);
  //  sprintf(spbuffer, "print_pinmux this pmuxtab=%0lx", this_pmux);
  //  mySerial.println(spbuffer);

  mySerial.print("PMUX");
  mySerial.print("(");
  mySerial.print(pmuxval);
  mySerial.print(") ");
  mySerial.print(this_pmux[port * 32 + pin]);
}
