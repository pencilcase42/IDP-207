
// set the motor speeds (left motor speed, right motor speed) (-255 -> +255)
void setMotors(int newLeft, int newRight){
  setLM(newLeft);
  setRM(newRight*0.928);// right motor slightly faster
}

// set left motor speed (-255 -> 255)
void setLM(int newSpeed){
  // only setting speed if different to previous one
  if (newSpeed==lmSpeed){
    return;
  }
  else{
      lmSpeed=newSpeed;
      if (lmSpeed>0){
        lm->run(FORWARD);
        lm->setSpeed(lmSpeed);
      } else if (lmSpeed==0){
        lm->run(RELEASE);
      } else if (lmSpeed<0){
        lm->run(BACKWARD);
        lm->setSpeed(-lmSpeed);
      }
  }
}

// set right motor speed (-255 -> 255)
void setRM(int newSpeed){
  // only setting speed if different to previous one
  if (newSpeed==rmSpeed){
    return;
  }
  else{
    rmSpeed=newSpeed;
    if (rmSpeed>0) {
      rm->run(FORWARD);
      rm->setSpeed(rmSpeed);
    } else if (rmSpeed==0){
      rm->run(RELEASE);
    } else if (rmSpeed<0){
      rm->run(BACKWARD);
      rm->setSpeed(-rmSpeed);
    }
  }
}
