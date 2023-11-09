
// get position on line
int getLP(){  
  //return -1, 0, 1
  int three[] = {valLeft, valForward, valRight};
  // centre
  if ((valLeft == 0) && (valForward == 1) && (valRight == 0)){
    return 0;
  }
  // on right
  else if (((valLeft == 1) && (valForward == 1) && (valRight == 0)) || ((valLeft == 1) && (valForward == 0) && (valRight == 0))){
    return 1;
  }
  // on left
  else if (((valLeft == 0) && (valForward == 1) && (valRight == 1)) || ((valLeft == 0) && (valForward == 0) && (valRight == 1))){
    return -1;
  }
  // no idea...
  else{
    return 2;
  }
}

// get junction type
int getJT(){
  // return none (line), left, right, end, quod

}
