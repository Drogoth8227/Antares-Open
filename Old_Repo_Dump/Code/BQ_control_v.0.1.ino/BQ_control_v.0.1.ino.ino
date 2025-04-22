static const uint8_t BQEN1 = PB0;
static const uint8_t BQEN2 = PB1;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(BQEN1, OUTPUT);
  pinMode(BQEN2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // en1 low + en2 low, 100ma
  // en1 high + en2 low, 500ma
  // en1 low + en2 high, set by external resistor
  // en1 high + en2 high, standby (usb suspend)
}
