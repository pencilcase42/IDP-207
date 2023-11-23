void lowerClaw(){
  clawm->run(FORWARD);
  clawm->setSpeed(100);
  delay(6000);
  clawm->setSpeed(0);
}

void raiseClaw(){
  clawm->run(BACKWARD);
  clawm->setSpeed(100);
  delay(6000);
  clawm->setSpeed(0);
}
void openClaw(){

  for (pos=clawServoClosed; pos >= clawServoOpen; pos -= 1) {
    clawServo.write(pos);
    delay(15);
  }
}

void closeClaw(){
  for (pos=clawServoOpen; pos <= clawServoClosed; pos += 1) {
    clawServo.write(pos);
    delay(15);
  }
}

