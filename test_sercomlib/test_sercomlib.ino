#include "sercomlib.h"
/*
 *    July 2020 by Bill Westfield.  Released to the public domain.
 *
 *   Attach the extra SERCOM peripherals to Serial Ports (UART mode.)
 *   Note that for Grand Central, the required PIN_SERIALn_RX/etc 
 *    defines are already in variant.h
 *   For other boards, it may be necessary to define them.
 */


//#define DEMO_I2C 1

//              SERCOM#, Serial#
sercom_UseSerial(  4,      2)   // Serial2 - pins 18 and 19
sercom_UseSerial(  1,      3)   // Serial3 - pins 16 and 17
sercom_UseSerial(  5,      4)   // Serial4 - pins 14 and 15
#ifdef DEMO_I2C
#include <Wire.h>
sercom_UseI2CPins( 7,   Wire7, 82, 81)  // Wire7 on SPI pins.
#else
sercom_UseSerialPins(3, SerialI2C, 21, 20)  // SerialI2C - I2C pins
#endif

void pullup (int pin) {
  PortGroup *port = digitalPinToPort(pin);
  port->OUTSET.reg = g_APinDescription[pin].ulPin ;
  port->PINCFG[g_APinDescription[pin].ulPin].reg |= PORT_PINCFG_PULLEN ;
}

void setup() {
  // Start ports at different speeds to demo their independence.
  Serial.begin(9600);

  Serial1.begin(9600);     // Serial1 (pins 0 and 1) alread defined.
  pullup(PIN_SERIAL1_RX);

  Serial2.begin(9600);
  pullup(PIN_SERIAL2_RX);

  Serial3.begin(9600);
  pullup(PIN_SERIAL3_RX);

  Serial4.begin(9600);
  pullup(PIN_SERIAL4_RX);

#ifdef DEMO_I2C
  Wire7.begin();
#else
  SerialI2C.begin(9600);
  pullup(21);
#endif
  
  while (!Serial)           // Wait for USB Serial to finish initialized
    ;
}

void loop() {
  Serial.println("This is Serial");
  Serial.println("1");
  Serial1.println("This a Serial 1");    // Check Transmit
  while (Serial1.available()) {          // Check receive
    Serial1.print("Serial 1 read ");
    Serial1.println(Serial1.read());
  }
  Serial.println("2");
  Serial2.println("This a Serial 2");
  while (Serial2.available()) {
    Serial2.print("Serial 2 read ");
    Serial2.println(Serial2.read());
  }
  Serial.println("3");

  Serial3.println("This a Serial 3");
  while (Serial3.available()) {
    Serial3.print("Serial 3 read ");
    Serial3.println(Serial3.read());
  }
  Serial.println("4");

  Serial4.println("This a Serial 4");
  while (Serial4.available()) {
    Serial4.print("Serial 4 read ");
    Serial4.println(Serial4.read());
  }
#ifndef DEMO_I2C
  Serial.println("I2C");
  SerialI2C.println("This a Serial I2C");
  while (SerialI2C.available()) {
    SerialI2C.print("Serial I2C read ");
    SerialI2C.println(SerialI2C.read());
  }
#endif
  delay(1000);

}
