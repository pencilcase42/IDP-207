typedef struct {
  int number;
  int north;
  int south;
  int east;
  int west;
} Node ;

Node nodes[13] = {{0, 1, -1, -1, -1},
                  {1, 6, -1, 2, 10},
                  {2, 5, -1, 3, 1},
                  {3, 4, 12, -1, 2},
                  {4, -1, 3, -1, 5},
                  {5, -1, 2, 4, 6},
                  {6, -1, 1, 5, 7},
                  {7, -1, 10, 6, 8},
                  {8, -1, 9, 7, -1},
                  {9, 8, 11, 10, -1},
                  {10, 7, -1, 1, 9},
                  {11, 9, -1, -1, -1},
                  {12, 3, -1, -1, -1}};

/*
int graph[12][12]={
		{0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
*/

int current_junction = 0; // initialises the arduino robot to be at the start
int next_junction = 0;
int i = 0; // tracking which stage of the standard path it is on
int j = 0; // tracking which stage of the home path it is on
int standardpath[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int homepaths[10][6] = {{1, 0, 0, 0, 0, 0},
                        {2, 1, 0, 0, 0, 0},
                        {3, 2, 1, 0, 0, 0},
                        {4, 3, 2, 1, 0, 0},
                        {5, 2, 1, 0, 0, 0},
                        {6, 1, 0, 0, 0, 0},
                        {7, 6, 1, 0, 0, 0},
                        {8, 7, 6, 1, 0, 0},
                        {9, 8, 7, 6, 1, 0},
                        {10, 1, 0, 0, 0, 0}};

String FindDirection() {
  String forward = "forward";
  String left = "left";
  String right = "right";
  String back = "back";
  if (nodes[current_junction].north == next_junction) {
      if (direction == "north") { direction="north"; return forward; }
      else if (direction == "east") { direction="north"; return left; }
      else if (direction == "west") { direction="north"; return right; }  
      else if (direction == "south") { direction="north"; return back; }  
  } else if (nodes[current_junction].east == next_junction) {
      if (direction == "north") { direction="east"; return right; }
      else if (direction == "east") { direction="east"; return forward; }
      else if (direction == "west") { direction="east"; return back; }  
      else if (direction == "south") { direction="east"; return left; } 
  } else if (nodes[current_junction].south == next_junction) {
      if (direction == "north") { direction="south"; return back; }
      else if (direction == "east") { direction="south"; return right; }
      else if (direction == "west") { direction="south"; return left; }  
      else if (direction == "south") { direction="south"; return forward; } 
  } else if (nodes[current_junction].west ==  next_junction) {
      if (direction == "north") { direction="west"; return left; }
      else if (direction == "east") { direction="west"; return back; }
      else if (direction == "west") { direction="west"; return forward; }  
      else if (direction == "south") { direction="west"; return right; } 
  } else {
      Serial.println("I dont know where to go");
  }
}

/*
void MoveInDirection(String direction_to_go) { 
  // function to turn left/right/go forward/go backward depending on the direction_to_go, and sets a new 
  // direction once a turn has been made
  // need to set junction_reached back to true once turning/moving is done
  if (direction_to_go == "front") {
    // ...
  } else if (direction_to_go == "left") {
    // ...
  }
}
*/


String junctionReached() { // block_found to be FALSE only after the block dropped off
  if (block_found == false) {
    current_junction = standardpath[i];
    next_junction = standardpath[i+1];
    i++;
    String direction_to_go = FindDirection();
    return direction_to_go;
  } else {
    int block_junction = standardpath[i];
    int path[6] = {homepaths[block_junction]};
    current_junction = path[j];
    if (current_junction != 0) {
      next_junction = path[j+1];
      j++;
      String direction_to_go = FindDirection();
      return direction_to_go;
    } else {
      i = 0;  // resetting current_junction by resetting values of i and j for the next round of search
      j = 0;
      return "magneticturn";
    }
  }
}

/*
void CarryOutPath(int* path) {
  int i = 0;
  bool junction_reached = false; 
  while (i < sizeof(path)) {
    if (junction_reached = true) {
      int current_node = path[i];
      int next_node = path[i+1];
      junction_reached = false;
      String direction_to_go = FindDirection(current_node, next_junction, direction);
      return direction_to_go;
      i++;
    }
}

void FindPath(current_node, end_node) {  
}
*/
/*
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CarryOutPath(startingpath);
  // once block found at a junction:
  // detect if magnetic or not: if magnetic, FindPath(current_node, 11); else FindPath(current_node, 12);
  FindPath(current_node, 11);
}

void loop() {
  // put your main code here, to run repeatedly:

}
*/