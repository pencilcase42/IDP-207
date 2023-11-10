#include <Serial.h>

// setup the 4x line sensors
void setLineSensorValues(){
  valLeft = digitalRead(leftPin);
  valRight = digitalRead(rightPin);
  valRearLeft = digitalRead(rLeftPin);
  valRearRight = digitalRead(rRightPin);
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

// get junction type
int getJT(){
  // return 0 (none), -1 (left T), 1 (right T), 2 (end)
  if ((valRLeft==0)&&(valRRight==0)){
    return 0;
  } else {
    Serial.println("Junction detected")
    return 3;
  }

}
