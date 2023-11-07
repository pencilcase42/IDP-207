void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  move();
}

void loop() {
   Serial.println("loop");
   delay(10000);
 
}
