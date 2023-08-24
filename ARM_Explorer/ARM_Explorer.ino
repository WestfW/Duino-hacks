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

#undef PINS_COUNT
#define PINS_COUNT 20
#define PERIPH_COUNT_IRQn 32

simpleParser<> ttycli(Serial);
extern "C" {
  extern void Default_Handler(void);
}


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
  Serial.print(F("ARM Explorer> "));
  Serial1.begin(9600);
  //  pinMode(PIN_WIRE_SDA, INPUT);
  //  attachInterrupt(PIN_WIRE_SDA, myCallback, CHANGE);
}

volatile boolean sawInterrupt = 0;

void myCallback() {
  sawInterrupt = true;
}

void loop() {
  backgroundMonitor();
}

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
      PSTR("board ""bytes ""hwords "   "words " "pwm " "interrupts " "Serial "   "pins " "scb " "fpu " "systick "   "help "    "? ");
    enum {
      CMD_BOARD, CMD_BYTE, CMD_HWORD, CMD_WORD, CMD_PWM, CMD_INT,    CMD_SERCOM, CMD_PINS, CMD_SCB, CMD_FPU, CMD_SYSTICK, CMD_HELP, CMD_HELP2  // make sure this matches the string
    };

    cmd = ttycli.keyword(cmd_strings); // look for a command.

    switch (cmd) {
      case CMD_BOARD:
#if 0
        SHOWSYMS(USB_MANUFACTURER);
        SHOWSYMS(USB_PRODUCT);
#endif
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
#define VECDEFAULT (0xFFFFFFFE & (uint32_t)Default_Handler)

      case CMD_SCB:
        print_scb();
        break;

      case CMD_FPU:
        print_fpu();
        break;

      case CMD_SYSTICK:
        print_systick();
        break;

      case CMD_INT:
        {
          int nInts;
#ifdef SCnSCB //v7m has data about interrupts
          sprintf(spbuffer, "  SCnSCB->ICTR= %08lx (%s)",
                  SCnSCB->ICTR, "Interrupt Controller info");
          mySerial.print(spbuffer);
          int field = ((SCnSCB->ICTR & SCnSCB_ICTR_INTLINESNUM_Msk) >> SCnSCB_ICTR_INTLINESNUM_Pos);
          nInts = (field + 1) * 32;
          sprintf(spbuffer, " (%ld extern Interrputs)\n", nInts);
          mySerial.print(spbuffer);
          sprintf(spbuffer, "  SCnSCB->ACTLR= %08lx (%s)\n", SCnSCB->ACTLR, "Auxiliary Control info");
          mySerial.print(spbuffer);
#else //v6m has 32 external interrupts.
          nInts = 32;
#endif
          uint32_t *vecs = (uint32_t *)(SCB->VTOR);
          sprintf(spbuffer, "\nException Table (@0x%lx)\n---------------\n", (uint32_t) vecs);
          mySerial.print(spbuffer);
          for (int i = 0; i < 16; i++) {
            uint32_t thisVec = vecs[i] & ~1;
            if (thisVec != 0) {
              Serial.print(vecnames[i]);
              Serial.print(" = 0x");
              Serial.print(thisVec, HEX);
              if (thisVec == VECDEFAULT) {
                Serial.print(" (default)");
              }
              Serial.println();
            }
          }
          for (int i = 0; i < nInts; i++) {
            uint32_t thisVec = vecs[i + 16] & ~1;
            if (thisVec != 0) {
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
        Serial.print("Not Implemented");
        Serial.println();
        break;

      case CMD_SERCOM:
        Serial.print("Not Implemented");
        Serial.println();
        break;

      case CMD_PINS:
        {
          int start = 0, end = PINS_COUNT;
          if ((n = ttycli.number()) >= 0) {
            start = n;
            end = start + 1;
          }
          mySerial.print("Not Implemented");
          mySerial.println();
        }
        break;


      case CMD_HELP2:
      case CMD_HELP:
        Serial.println(F("Enter a type of info.  One of : "));
        Serial.println(reinterpret_cast<const __FlashStringHelper *>(cmd_strings));
        break;

      case CMD_BYTE:
        {
          uint8_t *p;
          n = ttycli.number();
          p = (uint8_t *) n;
          if (n >= 0) {
            n = ttycli.number();
            if (n < 0) {
              n = 1;
            }
          } else break;

          while (n > 0) {
            sprintf(spbuffer, "0x%08lx : ", (uint32_t)p);
            Serial.print(spbuffer);
            for (int i = 0; i < 16; i++) {
              sprintf(spbuffer, " %02x", p[i]);
              Serial.print(spbuffer);
            }
            Serial.println();
            n -= 16;
            p += 16;
          }
        }
        break;

      case CMD_HWORD:
        {
          uint16_t *p;
          n = ttycli.number();
          p = (uint16_t *) n;
          if (n >= 0) {
            n = ttycli.number();
            if (n < 0) {
              n = 1;
            }
          } else break;

          while (n > 0) {
            sprintf(spbuffer, "0x%08lx : ", (uint32_t)p);
            Serial.print(spbuffer);
            for (int i = 0; i < 16; i++) {
              sprintf(spbuffer, " %04lx", p[i]);
              Serial.print(spbuffer);
            }
            Serial.println();
            n -= 16;
            p += 16;
          }
        }
        break;

      case CMD_WORD:
        {
          uint32_t *p;
          n = ttycli.number();
          p = (uint32_t *) n;
          if (n >= 0) {
            n = ttycli.number();
            if (n < 0) {
              n = 1;
            }
          } else break;

          while (n > 0) {
            sprintf(spbuffer, "0x%08lx : ", (uint32_t)p);
            Serial.print(spbuffer);
            for (int i = 0; i < 8; i++) {
              sprintf(spbuffer, " %08lx", p[i]);
              Serial.print(spbuffer);
            }
            Serial.println();
            n -= 8;
            p += 8;
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

    Serial.print(F("\nARM Explorer > "));
    ttycli.reset();
  }
}
void print_scb() {
  SCB_Type *p = SCB;
  mySerial.print("SCB (System Control Block @0x"); mySerial.println((uint32_t)SCB, HEX);
  sprintf(spbuffer, "%6s=0x%08lx%s", "CPUID", p->CPUID, " CPUID Base Register \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "ICSR", p->ICSR, " Interrupt Control and State  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "VTOR", p->VTOR, " Vector Table Offset  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "AIRCR", p->AIRCR, " Application Interrupt and Reset Control  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "SCR", p->SCR, " System Control  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "CCR", p->CCR, " Configuration Control  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s",  "SHP", p->SHP[0], " System Handlers Priority Registers (4-7, 8-11, 12-15) \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "SHCSR", p->SHCSR, " System Handler Control and State  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "CFSR", p->CFSR, " Configurable Fault Status  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "HFSR", p->HFSR, " HardFault Status  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "DFSR", p->DFSR, " Debug Fault Status  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "MMFAR", p->MMFAR, " MemManage Fault Address  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "BFAR", p->BFAR, " BusFault Address  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "AFSR", p->AFSR, " Auxiliary Fault Status  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "PFR", p->PFR[0], " Processor Feature  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "DFR", p->DFR, " Debug Feature  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "ADR", p->ADR, " Auxiliary Feature  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "MMFR", p->MMFR[0], " Memory Model Feature  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "ISAR", p->ISAR[0], " Instruction Set Attributes  \n");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s", "CPACR", p->CPACR, " Coprocessor Access Control  \n");
  mySerial.print(spbuffer);
}

void print_fpu() {
#if !defined(FPU)
  mySerial.println("No Floating Point is present.");
#else
  FPU_Type *p = FPU;
  sprintf(spbuffer, "FPU (Float Processing Unit info @%08lx)\n", (uint32_t)FPU);
  mySerial.print(spbuffer);

  sprintf(spbuffer, "  %6s=0x%08lx%s\n", "FPCCR", p->FPCCR, " FP Context Control ");
  if (p->FPCCR & FPU_FPCCR_LSPACT_Msk) {
    mySerial.print("    (Lazy State Preservation is ON)\n");
  }
  mySerial.print(spbuffer);
  sprintf(spbuffer, "  %6s=0x%08lx%s\n", "FPCAR", p->FPCAR, " FP Context Address ");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "  %6s=0x%08lx%s\n", "FPDSCR", p->FPDSCR, " FP Default Status Control ");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "  %6s=0x%08lx%s\n", "MVFR0", p->MVFR0, " Media and FP Feature  0");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "  %6s=0x%08lx%s\n", "MVFR1", p->MVFR1, " Media and FP Feature  1");
  mySerial.print(spbuffer);
#endif
}

void print_systick() {
  SysTick_Type *p = SysTick;
  Serial.println("SysTick Registers");
  sprintf(spbuffer, "%6s=0x%08lx%s\n", "CTRL", p-> CTRL, " Control and Status ");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=%8ld%s\n", "LOAD", p-> LOAD, " Reload Value ");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=%8ld%s\n", "VAL", p-> VAL, " Current Value ");
  mySerial.print(spbuffer);
  sprintf(spbuffer, "%6s=0x%08lx%s\n", "CALIB", p-> CALIB, " Calibration ");
  mySerial.print(spbuffer);
}

