#define IP_PROTO_UDP_V 0
#define IP_PROTO_TCP_V 1

uint8_t gPB[576];

/*
 * Improved ip checksum code, by Bill Westfield
 * So, back in the mid 1980s when MIT was first developing PC-IP, I noticed that
 * you could speed up the IP checksum algorithm by using loop instructions that
 * don't modify carry, and avoid the extra effort applied to handling end-around-carry
 * on (nearly all) microprocessors with extended precision add instructions ("adc")
 * Somewhat later, when assembler is being deprecated, we notice that explicit
 * manipulation of carry is NOT something that C can do, and perhaps that means
 * that the ip checksum can still benefit from being implemented in assembly language.
 * This inline-assembly version of the fill_checksum function from the Arduino EtherCard
 * library (and others) is less than half the size, and more than twice the speed, of
 * the original version...
 */
 uint16_t __attribute__((noinline)) fill_checksum2(uint8_t dest, uint8_t off, uint16_t len, uint8_t type);
 uint16_t fill_checksum2(uint8_t dest, uint8_t off, uint16_t len, uint8_t type) {
    const uint8_t* ptr = gPB + off;
    uint16_t sum = type==1 ? IP_PROTO_UDP_V+len-8 :
                   type==2 ? IP_PROTO_TCP_V+len-8 : 0;
    asm volatile(
  " cpi %A[len], 0  ; check length. incidentally clear carry \n\t"
  " brne 3f \n\t"
  "  dec %B[len]    ;  (lsb len of zero requires handling.) \n\t"
      "3: ld __tmp_reg__, %a0+    ;start with high byte (big-endian data) \n\t"
  " adc %B[sum], __tmp_reg__ \n\t"
  " dec %A[len]     ; 16bit decrement of count \n\t"
  " brne  0f    ;  note that dec does not affect carry. \n\t"
  "  dec %B[len] \n\t"
  "  brmi  1f \n"
      "0: ld __tmp_reg__, %a0+ \n\t"
  " adc %A[sum], __tmp_reg__ \n\t"
  " dec %A[len] \n\t"
  " brne 3b \n\t"
  "  dec %B[len] \n\t"
  "  brge 3b \n\t"
  " adc %B[sum], __zero_reg__ \n\t"
      "1: adc %A[sum], __zero_reg__ ;add last possible carry bit. \n\t"
  " adc %B[sum], __zero_reg__ \n\t"
// Again? " adc %A1, __zero_reg__ \n\t adc %B1, __zero_reg__ \n\t"
// Consider these all to be outputs, even though some are nominally inputs, since
// they get modified/trashed by the function
      : /* Outputs */ [ptr] "+e"(ptr), [sum] "+w"(sum), [len] "+w"(len)
      : /* Inputs */ 
  /* : Clobbers  "(ptr)" */
      );
    return ~sum;
}

 uint16_t __attribute__((noinline)) fill_checksum(uint8_t, uint8_t , uint16_t ,uint8_t);
 uint16_t fill_checksum(uint8_t dest, uint8_t off, uint16_t len,uint8_t type) {
    const uint8_t* ptr = gPB + off;
    uint32_t sum = type==1 ? IP_PROTO_UDP_V+len-8 :
                   type==2 ? IP_PROTO_TCP_V+len-8 : 0;
    while(len >1) {
        sum += (uint16_t) (((uint32_t)*ptr<<8)|*(ptr+1));
        ptr+=2;
        len-=2;
    }
    if (len)
        sum += ((uint32_t)*ptr)<<8;
    while (sum>>16)
        sum = (uint16_t) sum + (sum >> 16);
    uint16_t ck = ~ (uint16_t) sum;
    return(ck);
}

long reps = 0, failures = 0;
volatile int dest=0, off = 0, len = 60, type = IP_PROTO_TCP_V;

void setup() {
  uint16_t c1, c2;

  Serial.begin(9600);
  Serial.println(F("IP Checksum code test\nProvide random seed:"));
  while (Serial.read() < 0)
    ;
  randomSeed(micros());
  Serial.println(F("ok\n\nFirst let's check speed."));

  /* First, let's do some timings. */
  for (int i = 0; i < sizeof(gPB); i++) {
    /* Fill our "packet" with random data */
    gPB[i] = random(256);
  }
  unsigned long starttime = millis();
  for (int i=0; i < 100; i++) {
    for (int len = 40; len < sizeof(gPB); len += 5) {
     volatile uint16_t c1 =  fill_checksum(dest, 0, len, type);
    }
  }
  Serial.print(F("Original checksum time = "));
  Serial.println(millis() - starttime);
 
  starttime = millis();
  for (int i=0; i < 100; i++) {
    for (int len = 40; len < sizeof(gPB); len += 5) {
     volatile uint16_t c2 =  fill_checksum2(dest, 0, len, type);
    }
  }
  Serial.print(F("modified checksum time = "));
  Serial.println(millis() - starttime);
  Serial.println(F("\nNow compare original checksum results to new code.\n"));
#if 0
  for (int i = 0; i < 100; i++) {
    gPB[i] = 0xFF;
  }
  c1 =  fill_checksum(dest, off, 80, type);
  c2 =  fill_checksum2(dest, off, 80, type);
  if (c1 != c2) {
    Serial.println(F("Failure for all ones."));
  }
  c1 =  fill_checksum(dest, off, 81, type);
  c2 =  fill_checksum2(dest, off, 81, type);
  if (c1 != c2) {
    Serial.println(F("Failure for all ones."));
  }
#endif
}


void loop() {
  reps++;
  if ((reps & 0x1F) == 0) {
    Serial.print(F("Reps completed: "));
    Serial.println(reps);
    if (failures > 0) {
      Serial.print(F("****************  Failures: "));
      Serial.println(failures);
      Serial.println();
    }
  }
  for (int i = 0; i < sizeof(gPB); i++) {
    /* Fill our "packet" with random data */
    gPB[i] = random(256);
  }
    
  uint16_t c1, c2;
  for (int len = 40; len < sizeof(gPB); len += random(10)) {
    c1 =  fill_checksum(dest, off, len, type);
    c2 =  fill_checksum2(dest, off, len, type);
    if (c1 != c2) {
      failures++;
      Serial.print(F("\nChecksum mismatch! \nLength = "));
      Serial.println(len);
      Serial.print(F("Original Checksum algorithm: "));
      Serial.println(c1, HEX);
      Serial.print(F("Modified Checksum algorithm: "));
      Serial.println(c2, HEX);
      delay(1000);
    }
  }
}
