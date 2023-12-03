//Diagram of board - M (Magnetic drop off), H (Home), NM (Non magnetic drop off)
/*
      11 ------------------------------- 12
      |                                  |
      |                                  |
      |                                  |
      8 ----- 7 ------ 6 ------ 5 ------ 4
      |       |        |        |        |
      |       |        |        |        |
      9 ----- 10 ----- 1 ------ 2 ------ 3
      |                |                 |
      |                |                 |
   *------*         *--0---*          *------*
   |  M   |         |   H  |          | NM   |
   |      |         |      |          |      |
   *------*         *------*          *------*

*/

typedef struct {
  int number;
  int north;
  int south;
  int east;
  int west;
} Node ;

//defining the nodes
Node nodes[15] = {{0, 1, -1, -1, -1},
                  {1, 6, 0, 2, 10},
                  {2, 5, -1, 3, 1},
                  {3, 4, 14, -1, 2},
                  {4, 12, 3, -1, 5},
                  {5, -1, 2, 4, 6},
                  {6, -1, 1, 5, 7},
                  {7, -1, 10, 6, 8},
                  {8, 11, 9, 7, -1},
                  {9, 8, 13, 10, -1},
                  {10, 7, -1, 1, 9},
                  {11, -1, 8, 12, -1},
                  {12, -1, 4, -1, 11}};


int blockJunction;
// robot will go in a loop to find blocks on junctions
int standardpath[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
// robot will take different path to find the blocks in free space
int freespacepath[10] = {0, 1, 6, 7, 8, 11, 12, 4, 5, 6};
// paths to get from a node to node 0
int homepaths[12][6] = {{1, 0, 0, 0, 0, 0},
                        {2, 1, 0, 0, 0, 0},
                        {3, 2, 1, 0, 0, 0},
                        {4, 3, 2, 1, 0, 0},
                        {5, 2, 1, 0, 0, 0},
                        {6, 1, 0, 0, 0, 0},
                        {7, 6, 1, 0, 0, 0},
                        {8, 7, 6, 1, 0, 0},
                        {9, 8, 7, 6, 1, 0},
                        {10, 1, 0, 0, 0, 0},
                        {11, 8, 7, 6, 1, 0},
                        {12, 4, 5, 6, 1, 0}};
int path[6];

//find the direction the robot needs to go depending on current direction
String FindDirection() {
  String forward = "forward";
  String left = "left";
  String right = "right";
  String back = "back";
  if (nodes[currentJunction].north == nextJunction) {
      if (direction == "north") { direction="north"; return forward; }
      else if (direction == "east") { direction="north"; return left; }
      else if (direction == "west") { direction="north"; return right; }  
      else if (direction == "south") { direction="north"; return back; }  
  } else if (nodes[currentJunction].east == nextJunction) {
      if (direction == "north") { direction="east"; return right; }
      else if (direction == "east") { direction="east"; return forward; }
      else if (direction == "west") { direction="east"; return back; }  
      else if (direction == "south") { direction="east"; return left; } 
  } else if (nodes[currentJunction].south == nextJunction) {
      if (direction == "north") { direction="south"; return back; }
      else if (direction == "east") { direction="south"; return right; }
      else if (direction == "west") { direction="south"; return left; }  
      else if (direction == "south") { direction="south"; return forward; } 
  } else if (nodes[currentJunction].west ==  nextJunction) {
      if (direction == "north") { direction="west"; return left; }
      else if (direction == "east") { direction="west"; return back; }
      else if (direction == "west") { direction="west"; return forward; }  
      else if (direction == "south") { direction="west"; return right; } 
  } else {
    state="lost";
    Serial.println("ERROR: I don't know where to go");
  }
}

//when a junction is reached, this function is called to tell the robot where to go
String junctionReached() { 
  // if no block, coninue on pre defined path
  if (blockFound == false) {
    if (blocksReturned < 2) {
      currentJunction = standardpath[i];
      nextJunction = standardpath[i+1];
    } else {
      currentJunction = freespacepath[i];
      nextJunction = freespacepath[i+1];
    }
    i++;
    String direction_to_go = FindDirection();
    Serial.println(direction_to_go);
    return direction_to_go;
  // if holding a block, travel to node 0
  } else {
    if (blocksReturned < 2) {
      blockJunction = standardpath[i];
    } else {
      blockJunction = freespacepath[i+1];
      Serial.print("Block Junction: ");
      Serial.println(blockJunction);
    }
    //creating the path for node 0
    for (int k = 0; k < 6; k++) {
     path[k] = homepaths[blockJunction - 1][k];
    }
    Serial.print("Path: ");
    Serial.println(path[j]);
    currentJunction = path[j];
    
    if (currentJunction != 0) {
      nextJunction = path[j+1];
      j++;
      String direction_to_go = FindDirection();
      Serial.println(direction_to_go);
      return direction_to_go;
    //once node 0 reached, return magentic turn to signify drop off phase started, and change state accordingly in main
    } else {
      i = 0;  // resetting currentJunction by resetting values of i and j for the next round of search
      j = 0;
      return "magneticturn";
    }
  }
}