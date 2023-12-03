//get time of flight distance value
void setTOFValue(){
  dist = sensor.readRangeSingleMillimeters();
}

//get ultrasonic distance value
void setUltraValue(){
    // read the value from the sensor:
  sensity = analogRead(sensityPin);
  // turn the ledPin on
  ultraDist = sensity * MAX_RANG / ADC_SOLUTION;//
}

// determine magnetic type with magnetic sensors
bool getMagneticType(){
  // set magnetic
  val = digitalRead(magneticPin);
  if (val==HIGH){
    return true;
  } else {
    return false;
  }
}

// turn on the red LED if magnetic, and green if non-magnetic
void turnOnMagneticLED(){
  Serial.println(magnetic);
  if (magnetic) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(greenLED, HIGH);
  }
}

// turn off the magnet type LED
void turnOffMagneticLED(){
  Serial.println(magnetic);
  if (magnetic) {
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
  }
}