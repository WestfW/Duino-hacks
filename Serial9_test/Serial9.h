/*
  Serial9.h
   Unbuffered support for 9bit Serial
   2019 by Bill Westfield (WestfW)
   Released to the public domain.
*/

#ifndef Serial9_h
#define Serial9_h

#include <inttypes.h>
#include <Stream.h>

class Serial9 // : public Stream
{
  private:
    HardwareSerial *_hw;
    volatile uint8_t * _ucsra;
    volatile uint8_t * _ucsrb;
    volatile uint8_t *_ucsrc;
    volatile uint8_t *_udr;
    uint8_t _errors;

  public:
    // public methods
    Serial9(HardwareSerial* hws);
    ~Serial9();
    void begin();

    size_t write(uint16_t byte);
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(const uint16_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }


    int read();
    int available();
};

#endif
