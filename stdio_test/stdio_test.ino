#include <stdio.h>
extern "C" int __ftoa_engine (double val, char *buf, unsigned char prec, unsigned char maxdgs);

int fput(char c, FILE* f) {
  Serial.write( c);
  return 0;
}

int fget(FILE* f) {
  int i = Serial.read();
  if (i < 0) return _FDEV_ERR;
  return i;
}

void setup() {
  Serial.begin(9600);
  Serial.println("\ntesting testing testing");
  delay (5000);
  stdout = fdevopen(fput, fget);
}

char buffer[20];

void loop ()
{
#if 0
  __ftoa_engine(PI, buffer, 3, 10);
  for (byte i = 0; i < 16; i++) {
    printf("%02x ", buffer[i]);
  }
#endif
  Serial.println();
  long long x = 1 << 63;
  long y = 1 << 31;
  Serial.println ("TestStart\n");
  printf( "This is a test of something  %d  %d\n", x, y);
  printf("This is a test of something float %f\n", PI);

  Serial.println("TestEnd\n");
  delay(10000);
}


