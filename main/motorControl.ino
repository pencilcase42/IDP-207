
// set the motor speeds (left motor speed, right motor speed) (-255 -> +255)
void setMotors(int newLeft, int newRight){
  setLM(newLeft);
  setRM(newRight*0.9);
}

// set left motor speed (-255 -> 255)
void setLM(int newSpeed){
  if (newSpeed==lmSpeed){
    return;
  }
  else{
      //Serial.println("Changing LM:"+str(lmSpeed)+"->"+str(newSpeed));
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
  if (newSpeed==rmSpeed){
    return;
  }
  else{
    // Serial.println("Changing RM:"+str(rmSpeed)+"->"+str(newSpeed));
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
