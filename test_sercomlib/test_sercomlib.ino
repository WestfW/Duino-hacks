#include "sercomlib.h"
/*
 *  July 2020 by Bill Westfield.  Released to the public domain.
 */
/*
 *  Attach the extra SERCOM peripherals to Serial Ports (UART mode.)
 *  Note that for Grand Central, the required PIN_SERIALn_RX/etc defines are already in variant.h
 *  For other boards, it may be necessary to define them.
 */
//              SERCOM#, Serial#
sercom_UseSerial(  4,      2)   // Serial2 - pins 18 and 19
sercom_UseSerial(  1,      3)   // Serial3 - pins 16 and 17
sercom_UseSerial(  5,      4)   // Serial4 - pins 14 and 15


void setup() {
  // Start ports at different speeds to demo their independence.
  Serial.begin(9600);
  Serial1.begin(19200);     // Serial1 (pins 0 and 1) alread defined.
  Serial2.begin(38400);
  Serial3.begin(115200);
  Serial4.begin(9600);
  while (!Serial)           // Wait for USB Serial to finish initialized
    ;
}

void loop() {
  Serial.println("This is Serial");
  
  Serial1.println("This a Serial 1");    // Check Transmit
  while (Serial1.available()) {          // Check receive
    Serial1.print("Serial 1 read ");
    Serial1.println(Serial1.read());
  }
  Serial2.println("This a Serial 2");
  while (Serial2.available()) {
    Serial2.print("Serial 2 read ");
    Serial2.println(Serial2.read());
  }
  Serial3.println("This a Serial 3");
  while (Serial3.available()) {
    Serial3.print("Serial 3 read ");
    Serial3.println(Serial3.read());
  }
  Serial4.println("This a Serial 4");
  while (Serial4.available()) {
    Serial4.print("Serial 4 read ");
    Serial4.println(Serial4.read());
  }
  delay(1000);

}
