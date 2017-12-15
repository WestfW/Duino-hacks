void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

boolean delay_without_delaying(unsigned long time) {
  // return false if we're still "delaying", true if time ms has passed.
  // this should look a lot like "blink without delay"
  static unsigned long previousmillis = 0;
  unsigned long currentmillis = millis();
  if (currentmillis - previousmillis >= time) {
    previousmillis = currentmillis;
    return true;
  }
  return false;
}

boolean delay_without_delaying(unsigned long &since, unsigned long time) {
  // return false if we're still "delaying", true if time ms has passed.
  // this should look a lot like "blink without delay"
  unsigned long currentmillis = millis();
  if (currentmillis - since >= time) {
    since = currentmillis;
    return true;
  }
  return false;
}



void loop() {
  static unsigned long ledtime = 0;
  static unsigned long atime, btime, ctime, nltime;
  static int ledstate = false;
  if (delay_without_delaying(ledtime, 500)) {
    ledstate = !ledstate;
    digitalWrite(13, ledstate);
  }
  if (delay_without_delaying(atime, 100)) {
    Serial.print("A");
  }
  if (delay_without_delaying(btime, 200)) {
    Serial.print("B");
  }
  if (delay_without_delaying(ctime, 30)) {
    Serial.print("C");
  }
  if (delay_without_delaying(nltime, 1000)) {
    Serial.print("\n");
  }
}


