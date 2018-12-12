/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>
#include "timers.h"

// Xplained Pro pins

#define XPRO_1_08 0
#define XPRO_1_07 1
#define XPRO_1_14 2
#define XPRO_3_08 3
#define XPRO_1_09 4
#define XPRO_2_09 5 // (SW0)
#define XPRO_3_07 6
#define XPRO_MB_PWM 6
#define XPRO_MB_PF4 6 // Mikrobus PWM (TCB0/W0)
#define XPRO_1_13 7
#define XPRO_3_10 8 
#define XPRO_3_14 9
#define XPRO_MB_TX 9
#define XPRO_MB_PB0 9 // Mikrobus UART3
#define XPRO_3_13 10
#define XPRO_MB_RX 10
#define XPRO_MB_PB1 10 // Mikrobus UART3
#define XPRO_2_05 11
#define XPRO_2_15 12
#define XPRO_3_09 13
#define XPRO_MB_PE2 13 // Mikrobus GPIO
#define XPRO_3_05 14  // A0
#define XPRO_MB__PD0 14 // Mikrobus GPIO
#define XPRO_3_06 15
#define XPRO_1_03 16
#define XPRO_1_04 17
#define XPRO_2_03 18
#define XPRO_2_04 19 // A5
#define XPRO_1_05 20
#define XPRO_1_06 21
#define XPRO_1_12 22
#define XPRO_MB_SCL 22
#define XPRO_MB_PC3 22 // Mikrobus TWI0
#define XPRO_1_17 23
#define XPRO_MB_MISO 23
#define XPRO_MB_PA5 23 // Mikrobus SPI
#define XPRO_1_16 24
#define XPRO_MB_MOSI 24
#define XPRO_MB_PA4 24 // Mikrobus SPI
#define XPRO_3_03 25
#define XPRO_MB_AN 25 // MikroBus Analog (ADC0 AIN7)
#define XPRO_2_08 26 // (LED)
#define XPRO_2_07 27
#define XPRO_1_18 28
#define XPRO_MB_SCK 28 // Mikrobus SPI0
#define XPRO_MB_PA6 28 // Mikrobus SPI0
#define XPRO_1_15 29
#define XPRO_2_10 30
#define XPRO_1_10 31
#define XPRO_2_14 32 (tx)
#define XPRO_2_13 33 (rx)
#define XPRO_1_11 34
#define XPRO_MB_SDA 34
#define XPRO_MB_PC2 34 // Mikrobus TWI0
#define XPRO_3_15 35
#define XPRO_MB_PF2 35  // Mikrobus GPIO
#define XPRO_MB_SS 35  // Mikrobus SPI
#define XPRO_DGI_SS 36        // PF3
// 37 xtal
// 38 xtal
#define XPRO_3_04 39
#define XPRO_2_06 40 // RESET

#define NUM_DIGITAL_PINS            20
#define NUM_ANALOG_INPUTS           6
#define NUM_TESTPOINT_PINS			9
#define NUM_OCCUPIED_PINS			6 // (TOSC1/2, VREF, RESET, DEBUG USART Rx/Tx)
#define NUM_TOTAL_FREE_PINS			(NUM_DIGITAL_PINS + NUM_ANALOG_INPUTS + NUM_TESTPOINT_PINS)
#define NUM_TOTAL_PINS				(NUM_DIGITAL_PINS + NUM_ANALOG_INPUTS + NUM_TESTPOINT_PINS + NUM_OCCUPIED_PINS)
#define MAX_TIMER_PIN 12
#define ANALOG_INPUT_OFFSET			14
#define digitalPinToAnalogInput(p)  ((p < NUM_ANALOG_INPUTS) ? (p) : (p) - ANALOG_INPUT_OFFSET)

#define EXTERNAL_NUM_INTERRUPTS 	48

#define digitalPinHasPWM(p)         ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10)

// SPI on Mikrobus connector
#define SPI_MUX		(PORTMUX_SPI0_DEFAULT_gc)
#define PIN_SPI_MISO	  (XPRO_MB_MISO)
#define PIN_SPI_SCK	  (XPRO_MB_SCK)
#define PIN_SPI_MOSI	  (XPRO_MB_MOSI)
#define PIN_SPI_SS	  (XPRO_MB_SS)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

// TWI on Mikrobus connector
#define TWI_MUX		  (PORTMUX_TWI0_ALT1_gc)
#define PIN_WIRE_SDA        (XPRO_MB_SDA)
#define PIN_WIRE_SCL        (XPRO_MB_SCL)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// Serial:
// Xplained Pro Debug USART (available via the EDGB virtual COM port)
// USART1 on mega4809
// Mapped to HWSERIAL0 in Serial library
#define HWSERIAL0 (&USART1)
#define HWSERIAL0_DRE_VECTOR (USART1_DRE_vect)
#define HWSERIAL0_DRE_VECTOR_NUM (USART1_DRE_vect_num)
#define HWSERIAL0_RXC_VECTOR (USART1_RXC_vect)
#define HWSERIAL0_MUX (PORTMUX_USART1_DEFAULT_gc)
#define PIN_WIRE_HWSERIAL0_TX (26)
#define PIN_WIRE_HWSERIAL0_RX (27)

// Serial1 - Pin 9,10 (PB0,PB1)
// Xplained Pro MikroBus UART
// USART3 on mega4809
// Mapped to HWSERIAL1 in Serial library
#define HWSERIAL1 (&USART3)
#define HWSERIAL1_DRE_VECTOR (USART3_DRE_vect)
#define HWSERIAL1_DRE_VECTOR_NUM (USART3_DRE_vect_num)
#define HWSERIAL1_RXC_VECTOR (USART3_RXC_vect)
#define HWSERIAL1_MUX		  (PORTMUX_USART3_DEFAULT_gc)
#define PIN_WIRE_HWSERIAL1_RX (XPRO_MB_RX)
#define PIN_WIRE_HWSERIAL1_TX (XPRO_MB_TX)

#if 0
// Uno2 Spare USART available on testpoints
// USART0 on mega4809 (alternative pins)
// Mapped to HWSERIAL2 in Serial library
#define HWSERIAL2 (&USART0)
#define HWSERIAL2_DRE_VECTOR (USART0_DRE_vect)
#define HWSERIAL2_DRE_VECTOR_NUM (USART0_DRE_vect_num)
#define HWSERIAL2_RXC_VECTOR (USART0_RXC_vect)
#define HWSERIAL2_MUX (PORTMUX_USART0_ALT1_gc)
#define PIN_WIRE_HWSERIAL2_RX (23)
#define PIN_WIRE_HWSERIAL2_TX (24)
#endif


#define HWSERIAL3_MUX (PORTMUX_USART2_NONE_gc)

#define MUX_SPI			(SPI_MUX)
#define SPI_INTERFACES_COUNT	1

#define LED_BUILTIN 31

#define PIN_A0   (14)
#define PIN_A1   (15)
#define PIN_A2   (16)
#define PIN_A3   (17)
#define PIN_A4   (18)
#define PIN_A5   (19)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;

#define PINS_COUNT		(40u)

//NINA
#define NINA_GPIO0		(28u)
#define NINA_RESETN		(29u)
#define NINA_ACK		(36u)

#define SPIWIFI_SS		(35u)
#define SPIWIFI_ACK		NINA_ACK
#define SPIWIFI_RESET	(~NINA_RESETN)

//IMU
#define SPIIMU_SS		(30u)
#define SPIIMU_INT		(31u)

#ifdef ARDUINO_MAIN

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEGA4809 / ARDUINO

// Xplained Pro 4809
//
//                     (SCL)(SDA) (7)  (2)                 (R)  (3~) (6~)
//                 PA4  PA3  PA2  PA1  PA0  GND  VDD  UPDI PF6  PF5  PF4  PF3
//
//                  48   47   46   45   44   43   42   41   40   39   38   37
//              + ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ +
//        PA5   1|                                                             |36  PF2
//        PA6   2|                                                             |35  PF1 (TOSC2)
//        PA7   3|                                                             |34  PF0 (TOSC1)
//   (9~) PB0   4|                                                             |33  PE3 (8)
//  (10~) PB1   5|                                                             |32  PE2 (13)
//   (5~) PB2   6|                                                             |31  PE1 (12)
//        PB3   7|                          48pin QFN                          |30  PE0 (11~)
//   (Tx) PB4   8|                                                             |29  GND
//   (Rx) PB5   9|                                                             |28  AVDD
//        PC0  10|                                                             |27  PD7 (VREF)
//        PC1  11|                                                             |26  PD6
//        PC2  12|                                                             |25  PD5 (A5)
//               + ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ ____ +
//                  13   14   15   16   17   18   19   20   21   22   23   24
//
//                  PC3  VDD  GND  PC4  PC5  PC6  PC7  PD0  PD1  PD2  PD3  PD4
//                                 (1)  (0)  (4)       (A0) (A1) (A2) (A3) (A4)

//

const uint8_t PROGMEM digital_pin_to_port[] = {
	PC, // 0 PC5/USART1_Rx
	PC, // 1 PC4/USART1_Tx
	PA, // 2 PA0
	PF, // 3 PF5
	PC, // 4 PC6
	PB, // 5 PB2
	PF, // 6 PF4
	PA, // 7 PA1
	PE, // 8 PE3
	PB, // 9 PB0
	PB, // 10 PB1
	PE, // 11 PE0
	PE, // 12 PE1
	PE, // 13 PE2
	PD, // 14 PD0/AI0
	PD, // 15 PD1AI1
	PD, // 16 PD2/AI2
	PD, // 17 PD3/AI3
	PD, // 18 PD4/AI4
	PD, // 19 PD5/AI5
	PA, // 20 PA2/TWI_SDA
	PA, // 21 PA3/TWI_SCL
	PC, // 22 PC3
	PA, // 23 PA5/NINA TX
	PA, // 24 PA4/NINA RX
	PD, // 25 PD6
	PC, // 26 PC0 Usart1 tx
	PC, // 27 PC1/USART1 rx (edbg)
	PA,	// 28 PA6/NINA GPIO0
	PA,	// 29 PA7/NINA RST
	PB,	// 30 PB3/IMU CS
	PB,	// 31 PB5 LED_BUILTIN
	PC,	// 32 PC0/MOSI
	PC,	// 33 PC1/MISO
	PC,	// 34 PC2/SCK
	PF,	// 35 PF2/NINA CS
	PF,	// 36 PF3/NINA ACK
	PF,	// 37 PF0 TOSC 1
	PF,	// 38 PF1 TOSC 2
	PD,	// 39 PD7 VREF
	PF	// 40 PF6 RESET
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_position[] = {
	PIN5_bp,	// 0 PC5/USART1_Rx
	PIN4_bp,	// 1 PC4/USART1_Tx
	PIN0_bp,	// 2 PA0
	PIN5_bp,	// 3 PF5
	PIN6_bp,	// 4 PC6
	PIN2_bp,	// 5 PB2
	PIN4_bp,	// 6 PF4
	PIN1_bp,	// 7 PA1
	PIN3_bp,	// 8 PE3
	PIN0_bp,	// 9 PB0
	PIN1_bp,	// 10 PB1
	PIN0_bp,	// 11 PE0
	PIN1_bp,	// 12 PE1
	PIN2_bp,	// 13 PE2
	PIN0_bp,	// 14 PD0/AI0
	PIN1_bp,	// 15 PD1AI1
	PIN2_bp,	// 16 PD2/AI2
	PIN3_bp,	// 17 PD3/AI3
	PIN4_bp,	// 18 PD4/AI4
	PIN5_bp,	// 19 PD5/AI5
	PIN2_bp,	// 20 PA2/TWI_SDA
	PIN3_bp,	// 21 PA3/TWI_SCL
	PIN3_bp,	// 22 PC3
	PIN5_bp,	// 23 PA5/NINA TX
	PIN4_bp,	// 24 PA4/NINA RX
	PIN6_bp,	// 25 PD6
	PIN0_bp,	// 26 PC0 usart 1tx (edbg)
	PIN1_bp,	// 27 PC1/USART1_rx (edbg)
	PIN6_bp,	// 28 PA6/NINA GPIO0
	PIN7_bp,	// 29 PA7/NINA RST
	PIN3_bp,	// 30 PB3/IMU CS
	PIN5_bp,	// 31 PB5 LED_BUILTIN
	PIN0_bp,	// 32 PC0/MOSI
	PIN1_bp,	// 33 PC1/MISO
	PIN2_bp,	// 34 PC2/SCK
	PIN2_bp,	// 35 PF2/NINA CS
	PIN3_bp,	// 36 PF3/NINA ACK
	PIN0_bp,	// 37 PF0 TOSC 1
	PIN1_bp,	// 38 PF1 TOSC 2
	PIN7_bp,	// 39 PD7 VREF
	PIN6_bp 	// 40 PF6 RESET
};

/* Use this for accessing PINnCTRL register */
const uint8_t PROGMEM digital_pin_to_bit_mask[] = {
	PIN5_bm,	// 0 PC5/USART1_Rx
	PIN4_bm,	// 1 PC4/USART1_Tx
	PIN0_bm,	// 2 PA0
	PIN5_bm,	// 3 PF5
	PIN6_bm,	// 4 PC6
	PIN2_bm,	// 5 PB2
	PIN4_bm,	// 6 PF4
	PIN1_bm,	// 7 PA1
	PIN3_bm,	// 8 PE3
	PIN0_bm,	// 9 PB0
	PIN1_bm,	// 10 PB1
	PIN0_bm,	// 11 PE0
	PIN1_bm,	// 12 PE1
	PIN2_bm,	// 13 PE2
	PIN0_bm,	// 14 PD0/AI0
	PIN1_bm,	// 15 PD1AI1
	PIN2_bm,	// 16 PD2/AI2
	PIN3_bm,	// 17 PD3/AI3
	PIN4_bm,	// 18 PD4/AI4
	PIN5_bm,	// 19 PD5/AI5
	PIN2_bm,	// 20 PA2/TWI_SDA
	PIN3_bm,	// 21 PA3/TWI_SCL
	PIN3_bm,	// 22 PC3
	PIN5_bm,	// 23 PA5/NINA TX
	PIN4_bm,	// 24 PA4/NINA RX
	PIN6_bm,	// 25 PD6
	PIN0_bm,	// 26 PC0
	PIN1_bm,	// 27 PC1
	PIN6_bm,	// 28 PA6/NINA GPIO0
	PIN7_bm,	// 29 PA7/NINA RST
	PIN3_bm,	// 30 PB3/IMU CS
	PIN5_bm,	// 31 PB5 LED_BUILTIN
	PIN0_bm,	// 32 PC0/MOSI
	PIN1_bm,	// 33 PC1/MISO
	PIN2_bm,	// 34 PC2/SCK
	PIN2_bm,	// 35 PF2/NINA CS
	PIN3_bm,	// 36 PF3/NINA ACK
	PIN0_bm,	// 37 PF0 TOSC 1
	PIN1_bm,	// 38 PF1 TOSC 2
	PIN7_bm,	// 39 PD7 VREF
	PIN6_bm 	// 40 PF6 RESET
};

const uint8_t PROGMEM digital_pin_to_timer[] = {
	NOT_ON_TIMER,	// 0 PC5/USART1_Rx
	NOT_ON_TIMER,	// 1 PC4/USART1_Tx
	NOT_ON_TIMER,	// 2 PA0
	TIMERB1,		// 3 PF5
	NOT_ON_TIMER,	// 4 PC6
	TIMERA0,		// 5 PB2
	TIMERB0,		// 6 PF4
	NOT_ON_TIMER,	// 7 PA1
	NOT_ON_TIMER,	// 8 PE3
	TIMERA0,		// 9 PB0
	TIMERA0,		// 10 PB1
	TIMERB2,		// 11 PE0
	NOT_ON_TIMER,	// 12 PE1
	NOT_ON_TIMER,	// 13 PE2
	NOT_ON_TIMER,	// 14 PD0/AI0
	NOT_ON_TIMER,	// 15 PD1AI1
	NOT_ON_TIMER,	// 16 PD2/AI2
	NOT_ON_TIMER,	// 17 PD3/AI3
	NOT_ON_TIMER,	// 18 PD4/AI4
	NOT_ON_TIMER,	// 19 PD5/AI5
	NOT_ON_TIMER,	// 20 PA2/TWI_SDA
	NOT_ON_TIMER,	// 21 PA3/TWI_SCL
	NOT_ON_TIMER,	// 22 PC3
	NOT_ON_TIMER,	// 23 PA5/NINA TX
	NOT_ON_TIMER,	// 24 PA4/NINA RX
	NOT_ON_TIMER,	// 25 PD6
	NOT_ON_TIMER,	// 26 PC0/USART1_tx
	NOT_ON_TIMER,	// 27 PC1/USART1_rx
	NOT_ON_TIMER,	// 28 PA6/NINA GPIO0
	NOT_ON_TIMER,	// 29 PA7/NINA RST
	NOT_ON_TIMER,	// 30 PB3/IMU CS
	NOT_ON_TIMER,	// 31 PB5 LED_BUILTIN
	NOT_ON_TIMER,	// 32 PC0/MOSI
	NOT_ON_TIMER,	// 33 PC1/MISO
	NOT_ON_TIMER,	// 34 PC2/SCK
	NOT_ON_TIMER,	// 35 PF2/NINA CS
	NOT_ON_TIMER,	// 36 PF3/NINA ACK
	NOT_ON_TIMER,	// 37 PF0 TOSC 1
	NOT_ON_TIMER,	// 38 PF1 TOSC 2
	NOT_ON_TIMER,	// 39 PD7 VREF
	NOT_ON_TIMER,	// 40 PF6 RESET
};

#endif // ARDINO_MAIN

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_MONITOR		Serial
#define SERIAL_PORT_HARDWARE		Serial1
#define SERIAL_PORT_MB			Serial1
#define SERIAL_PORT_USBVIRTUAL		Serial

#define SERIAL_PORT_HARDWARE_OPEN	Serial2

#endif
