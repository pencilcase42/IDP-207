//lower the claw
void lowerClaw(){
  magnetic=false;
  clawm->run(FORWARD);
  clawm->setSpeed(150);
  delay(5000);
  clawm->setSpeed(0);
}

//raise the claw
void raiseClaw(){
  clawm->run(BACKWARD);
  clawm->setSpeed(150);
  delay(5000);
  clawm->setSpeed(0);
}

// open the claw
void openClaw(){
  for (pos=clawServoClosed; pos >= clawServoOpen; pos -= 1) {
    clawServo.write(pos);
    delay(15);
  }
}

//close the claw
void closeClaw(){
  bool magneticDetected=false;
  for (pos=clawServoOpen; pos <= clawServoClosed; pos += 1) {
    if (getMagneticType()){magneticDetected=true;}
    clawServo.write(pos);
    delay(15);
  }
  if (magneticDetected){magnetic=true;}
}

// calibrating the claw by pressing a button to define its lowest point, and then making it go back up so it does not interefere with the sensors
void calibrateClawHeight(){
  // raise until green button pressed
  clawm->run(BACKWARD);
  clawm->setSpeed(100);
  while (digitalRead(greenButton)==LOW) {
    continue;
  }
  Serial.println("INFO: Green pressed, stop raising...");
  clawm->setSpeed(0);
  delay(1000);
  // lower until green button pressed
  clawm->run(FORWARD);
  clawm->setSpeed(100);
  while (digitalRead(greenButton)==LOW){
    continue;
  }
  Serial.println("INFO: Green pressed, stop lowering...");
  clawm->setSpeed(0);
  // now at bottom
  // raise back up
  raiseClaw();
}
