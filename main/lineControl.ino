
// setup the 4x line sensors
void setLineSensorValues(){
  valLeft = digitalRead(leftPin);
  valRight = digitalRead(rightPin);
  valRLeft = digitalRead(rLeftPin);
  valRRight = digitalRead(rRightPin);
}

// get position on line
int getLP(){  
  //return -1, 0, 1, 2 (left, centre, right, none)
  // centre
  if ((valLeft==1)&&(valRight==1)){
    return 0;
  // on right
  } else if ((valLeft==1)&&(valRight==0)){
    return 1;
  // on left
  } else if ((valLeft==0)&&(valRight==1)){
    return -1;
  // no line detected (valLeft=valRight==0)
  } else{
    return 2;
  }
}

bool foundJunction(){
  setLineSensorValues();
  if ((valRLeft == 1) || (valRRight == 1)){
    return true; 
  }else{
    return false;
  }
}

void straightenRear() {
  while ((valRLeft == 0) && (valRRight == 1)){
      Serial.println("INFO: straighten rear - rotate right");
      setLineSensorValues();
      setMotors(130,0);
  }
  while ((valRLeft == 1) && (valRRight == 0)){
      Serial.println("INFO: straighten rear - rotate left");
      setLineSensorValues();
      setMotors(0,130);
  }
}
