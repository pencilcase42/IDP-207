void forwards(){
  Serial.println("Move forward!");
  lm->run(FORWARD);
  rm->run(FORWARD);
  lm->setSpeed(255);
  rm->setSpeed(255);
}

void halt(){
  Serial.println("Stop!");
  lm->run(RELEASE);
  rm->run(RELEASE);
}
  
void backwards(){
  Serial.println("Move backward!");
  lm->run(BACKWARD);
  rm->run(BACKWARD);
  lm->setSpeed(255);
  rm->setSpeed(255);
}

void rotateLeft(){
  Serial.println("Rotate left!");
  lm->run(BACKWARD);
  rm->run(FORWARD);
  lm->setSpeed(255);
  rm->setSpeed(255);
}

void rotateRight(){
  Serial.println("Rotate right!");
  lm->run(FORWARD);
  rm->run(BACKWARD);
  lm->setSpeed(255);
  rm->setSpeed(255); 
}
