/*
 * sercomlib.h
 * July 2020, by Bill Westfield
 * Released to the public domain.
 * 
 * This defines macros to pair up "un-used" sercom interfaces with Arduino driver-level
 *  definitions like UART/HardwareSerial, on SAMD51-based boards.
 *  
 *  Use it like:
 *     sercom_UseSerial(sercomNumber, SerialNumber)
 *  It's all relatively ugly pre-processor magic, and it
 *  assumes that symbols like sercomN, (from variant.cpp)
 *  and PIN_SERIALn_TX, PIN_SERIALn_TX (from variant.h)
 *  are already defined (True for Adafruit's Grand Central Station board,
 *  but not for some others.
 *  SerialN and the SERCOMn_x_Handler() end up being defined.
 *     
 *  At some point, this might get expanded to allow I2C and SPI as well,
 *  and support other chip families.
 */
#include <variant.h>
#define sercom_UseSerial(_sercom, _serialNum) \
  Uart Serial##_serialNum( &sercom##_sercom, PIN_SERIAL##_serialNum##_RX, PIN_SERIAL##_serialNum##_TX, SERCOM_RX_PAD_1, UART_TX_PAD_0 ) ; \
  void SERCOM##_sercom##_0_Handler(void) {  Serial##_serialNum.IrqHandler(); } \
  void SERCOM##_sercom##_1_Handler(void) {  Serial##_serialNum.IrqHandler(); } \
  void SERCOM##_sercom##_2_Handler(void) {  Serial##_serialNum.IrqHandler(); } \
  void SERCOM##_sercom##_3_Handler(void) {  Serial##_serialNum.IrqHandler(); }
