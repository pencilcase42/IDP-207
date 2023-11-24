
void setDistanceValue(){
  dist = sensor.readRangeSingleMillimeters();
}

void findMagneticType(){
  // set magnetic
  val = digitalRead(magneticPin);
  if (val==HIGH){
    magnetic = true;
  } else {
    magnetic = false;
  }
}

void turnOnMagneticLED(){
  Serial.println(magnetic);
  if (magnetic) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(greenLED, HIGH);
  }
}

void turnOffMagneticLED(){
  Serial.println(magnetic);
  if (magnetic) {
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
  }
}