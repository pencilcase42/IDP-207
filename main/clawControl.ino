void lowerClaw(){
  clawm->run(BACKWARD);
  clawm->setSpeed(50);
  delay(3000);
}

void raiseClaw(){
  clawm->run(FORWARD);
  clawm->setSpeed(50);
  delay(3000);
}
void closeClaw(){

  for (pos=clawServoClosed; pos <= clawServoOpen; pos -= 1) {
    clawServo.write(pos);
    delay(15);
  }
}

void openClaw(){
  for (pos=clawServoOpen; pos <= clawServoClosed; pos += 1) {
    clawServo.write(pos);
    delay(15);
  }
}

