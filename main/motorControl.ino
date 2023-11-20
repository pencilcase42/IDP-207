// #include <Serial.h>


// motor testing procedure
// void motorTest(){
//   Serial.println("Motot testtt");
//   forwards();
//   delay(2000);
//   halt();
//   delay(2000);
//   backwards();
//   delay(2000);
//   halt();
//   delay(2000);
//   rotateRight();
//   delay(2000);
//   rotateLeft();
//   delay(2000);
//   halt();
// }

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
        lm->run(BACKWARD);
        lm->setSpeed(lmSpeed);
      } else if (lmSpeed==0){
        lm->run(RELEASE);
      } else if (lmSpeed<0){
        lm->run(FORWARD);
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
      rm->run(BACKWARD);
      rm->setSpeed(rmSpeed);
    } else if (rmSpeed==0){
      rm->run(RELEASE);
    } else if (rmSpeed<0){
      rm->run(FORWARD);
      rm->setSpeed(-rmSpeed);
    }
  }
}

// void forwards(){
//   Serial.println("Move forward!");
//   lm->run(FORWARD);
//   rm->run(FORWARD);
//   lm->setSpeed(255);
//   rm->setSpeed(255);
// }

// void halt(){
//   Serial.println("Stop!");
//   lm->run(RELEASE);
//   rm->run(RELEASE);
// }
  
// void backwards(){
//   Serial.println("Move backward!");
//   lm->run(BACKWARD);
//   rm->run(BACKWARD);
//   lm->setSpeed(255);
//   rm->setSpeed(255);
// }

// void rotateLeft(){
//   Serial.println("Rotate left!");
//   lm->run(BACKWARD);
//   rm->run(FORWARD);
//   lm->setSpeed(255);
//   rm->setSpeed(255);
// }

// void rotateRight(){
//   Serial.println("Rotate right!");
//   lm->run(FORWARD);
//   rm->run(BACKWARD);
//   lm->setSpeed(255);
//   rm->setSpeed(255); 
// }
