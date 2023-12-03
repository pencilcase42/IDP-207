#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>
#define MAX_RANG (520)//the max measurement value of the module is 520cm(a little bit longer than effective max range)
#define ADC_SOLUTION (1023.0)//ADC accuracy of Arduino UNO is 10bit

// MOTORS //
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(2);
Adafruit_DCMotor *rm = AFMS.getMotor(3);
Adafruit_DCMotor *clawm = AFMS.getMotor(4);

// motor speeds (0-255)
int lmSpeed=0;
int rmSpeed=0;

// claw servo
Servo clawServo;
const int clawServoPin = 10;
int clawServoOpen = 30; 
int clawServoClosed = 135;  
int pos;


// SENSORS // 

// Time of Flight Sensor
uint16_t dist;
VL53L0X sensor;

// Ultrasonic Sensor
int sensityPin = A0; 
float ultraDist = 500000, sensity;

// Magnetic Sensor
const int magneticPin = 11;
int val = 0;

// light sensor pins (front and rear (r))
const int rLeftPin = 5;
const int rRightPin = 4;
const int leftPin = 3;
const int rightPin = 2;

// light sensor values (1 is white, 0 is black)
int valLeft, valRight, valRLeft, valRRight;

// button pin#
const int greenButton = 9;


// LEDS //
const int blueLED = 6;
const int redLED = 7;
const int greenLED = 8;


// STATES //

// Global state
String state;
String previousState;

// Direction
String direction="north";

// Turning
const int ninetyTurn = 1280;  // time to rotate ninety seconds
String turningDirection;
bool turning=false;

// T Junction
unsigned long startTJtimer;
bool foundTJ=false;

// Leaving Junction
unsigned long startOutJTimer;
bool leavingJ=false;

// Home Junction
const int halfSquareTime=870; // time to move through half of home square
unsigned long startTimerStraighten;

// Block
bool blockFound = false;
bool magnetic = false;
int blocksReturned = 0;

// Junction Brain
int currentJunction = 0; // initialises the arduino robot to be at the start
int nextJunction = 0;
int i = 0; // tracking which stage of the standard path it is on
int j = 0; // tracking which stage of the home path it is on

void setup() {

  // SETUP //

  // init serial
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Initialising test...");

  // check Motor Shield
  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Motor Shield: NOT found, Check wiring.");
    while (1);
  } Serial.println("Motor Shield: found...");

  // init light sensors
  Serial.println("Light sensors: init..");
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(rLeftPin, INPUT);
  pinMode(rRightPin, INPUT);

  // init led
  Serial.println("LEDs: init...");
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  // init button
  Serial.println("Button: init...");
  pinMode(greenButton, INPUT);

  // init magnetic
  Serial.println("Magnetic: init...");
  pinMode(magneticPin, INPUT);

  // init servo
  Serial.println("Claw Servo: init...");
  clawServo.attach(clawServoPin);

  // init ToF
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init()){
    Serial.println("Failed to detect and initialize time of flight sensor!");
  } else {
    Serial.println("Time of Flight: init DONE...");
  }

  // Initial State //
  state="kill";

  // Claw calibration, so that it always starts in the raised position
  Serial.println("INFO: Calibrating claw height - raise claw, then lower claw");
  openClaw();
  calibrateClawHeight();
}

//for testing
void resetToJ0BlockState(){
  setMotors(0,0);
  delay(1000);

  // state="start home";
  state="go middle of home";

  previousState="";

  //direction="north";
  direction="south";

  foundTJ=false;
  turning=false;
  leavingJ=false;

  //blockFound=false;
  blockFound=true;

}

// resest to initial state once green button pressed, redefine all variables
void resetToHomeState(){
  setMotors(0,0);
  delay(1000);
  state="start home";
  previousState="";
  direction="north";
  foundTJ=false;
  turning=false;
  leavingJ=false;
  blockFound=false;
  currentJunction = 0;
  nextJunction = 0;
  i = 0;
  j = 0; 
  blocksReturned = 0; 
}

//reset to node 0 
void resetToJState(){
  setMotors(0,0);
  delay(1000);
  state="junction";
  previousState="";
  direction="north";
  foundTJ=false;
  turning=false;
  leavingJ=false;
  blockFound=false;
}

// make the blue LED light up at a frequency of 2Hz
void updateBlueLED(){
  if (lmSpeed==0 && rmSpeed==0){
    digitalWrite(blueLED, LOW);
  } else {
    digitalWrite(blueLED, (millis() / 500) % 2);
  }
}

// The idea was to have a number of states that the robot would be in, so that program would loop as many times as possible, thus updating sensor values more often
void loop() {
  // only check the distance sensors if not holding a block
  if (blockFound){
    dist = 500000;
    ultraDist = 500000;
  } else{
    setTOFValue();
    if (blocksReturned >= 2){
      setUltraValue();
    }

  }

  // Output current state
  if (previousState!=state) {
    Serial.print("STATE: ");
    Serial.println(state);
    previousState=state;
  }

  // retrieved 1(white) or 0(black) from the line sensors
  setLineSensorValues();

  // Kill/reset button (kill first hit, reset second hit)
  if (digitalRead(greenButton) == HIGH){
    if (state=="kill") {
      Serial.println("INFO: button hit - 2=reset");
      resetToHomeState();
    } else {
      state="kill";
      Serial.println("INFO: green button hit - 1=kill");
      delay(1000);
    }
  }

  updateBlueLED();

  if (state=="kill"){
    // stop
    setMotors(0,0);

  } else if (state=="lost"){
    // stop
    setMotors(0,0);

  } else if (state =="line"){
    if (foundJunction()){
      //stop when a junction is found and change state
      setMotors(0,0);
      state="junction";
    }else{
      // LINE FOLLOWING:
      // determine line position
      int lp=getLP();
      // if left of line
      if (lp==-1){
        setMotors(240,0);
      // if on line
      } else if (lp==0){
        setMotors(200,200);
        //when straight, check distance sensors to see if block close by
        if ((dist<67) && !blockFound) {
          //if block in front, stop and change state
          setMotors(0,0);
          blockFound=true;
          Serial.print("INFO: Time of Flight=");
          Serial.println(dist);
          state = "block and pickup";
        }
        if ((ultraDist<50) && !blockFound){
          // if block in free space, stop, move forward since the ultrasound sensor does not line up with wheels, then change state
          Serial.print("INFO: ULTRA = ");
          Serial.println(ultraDist);
          setMotors(0,0);
          delay(1000);
          setMotors(200,200);
          delay(1045);
          setMotors(0,0);
          state = "freespace turn";
        }
      // if right of line
      } else if (lp==1){
        setMotors(0,240);
      } else if (lp==2){
        // T JUNCTION
        //if both front sensors are black, assume it is a T junction
        state="TJ";  
      }
    }

  } else if (state=="block and pickup"){

    // pick up
    Serial.println("INFO: Picking Up block");
    lowerClaw();
    closeClaw();  // magnetic type detection happens here

    // show detection, raise claw while doing this
    turnOnMagneticLED();    
    raiseClaw();  // includes 5s delay
    turnOffMagneticLED();

    // return to line state or freespace turn depending on number of blocks already picked up
    if (blocksReturned >=2){
      state = "freespace return";
    } else{
      state="line";
    }
  } else if (state == "freespace turn"){
    //rotate to travel towards block in free space
    setMotors(150,-150);
    delay(ninetyTurn);
    state = "freespace move1";

  } else if (state == "freespace move1"){
      //move straight in free space until close to block , then change state
      if ((dist>75) && !blockFound){
        setMotors(200,200);
      } else {
      setMotors(0,0);
      blockFound = true;
      state = "block and pickup";
      }
  } else if (state == "freespace return"){
    // when picked up block in free space, rotate 180 degrees and get back to line, then go back to line state
      setMotors(150,-150);
      delay(ninetyTurn*2.17);
      setMotors(200,200);
      while (!foundJunction()){
        setLineSensorValues();
        updateBlueLED();
        continue;
      }
      setMotors(0,0);
      delay(2000);
      
      while (getLP() == 2){
        setLineSensorValues();
        updateBlueLED();
        setMotors(150,-150);
      }
      state = "line";
  } else if (state == "TJ"){
    if (!foundTJ){
      startTJtimer=millis();   
      foundTJ=true;      
    }
    if (foundJunction()) {
      // found, leave loop
      foundTJ=false;
      state="junction";
    } else if ((millis() - startTJtimer) < 3000) {
      // not found, but yes keep looking
      setMotors(150,150); 
    } else {
      Serial.println("ERROR: abort looking for TJ");
      // not found, lost mode
      foundTJ=false;
      state="lost";
    }
  } else if (state == "junction"){
    if (!turning) { // if first time at junction
      turningDirection = junctionReached();
      Serial.println(turningDirection);
      if (turningDirection == "forward"){
        state = "leaving junction";
        turning = false;
      } else if (turningDirection == "right"){
          setMotors(150,-150);
          delay(500);
          turning = true;
      } else if (turningDirection == "left"){
          setMotors(-150,150);
          delay(500);
          turning = true;
      } else if (turningDirection == "back"){
          setMotors(-150,150);
          delay(ninetyTurn*1.9);
          turning = true;
      } else if (turningDirection == "magneticturn"){
          turning = false;
          state = "go middle of home";
      }
    } else if (turning) {
      if((valLeft == 0) || (valRight == 0)){
        // continue
      } else{
        // line found, leave junction
        turning = false;
        state = "leaving junction";
      }
    }
  
  } else if (state == "leaving junction"){
    if (!leavingJ){
      leavingJ=true;
      startOutJTimer = millis();
    } else if (millis()-startOutJTimer<800){
      int lp=getLP();
      if (lp==-1){
        setMotors(240,0);
        // if on line
      } else if (lp==0){
        setMotors(200,200);
        // if right of line
      } else if (lp==1){
        setMotors(0,240);
        // if not on line -> stop
      }
    } else {
      // clear of junction, now no longer leaving J
      leavingJ=false;
      state="line"; // once left junction go back to line state
    }
  } else if (state == "start home"){
    // starting state of robot, when button pressed, will move until finds node 0
    if (!foundJunction()){
      setMotors(150,150);
    } else if (foundJunction()){
      state = "junction";
    }
  } else if (state == "go middle of home"){
    // start of drop off from node 0
    //get out of j0 and go to middle of home
    setMotors(200,200);
    delay(790);
    state = "turn in home";
  } else if (state=="turn in home") {
    // check if magnetic or not and initiate turn
    if (magnetic){
      setMotors(150,-150);
    } else {
      setMotors(-150,150);
    }
    //delay for 90 degree turn 
    delay(ninetyTurn);
    state = "move to edge";
  } else if (state=="move to edge"){
    // find edge of home square 
    setMotors(200,200);
    if (foundJunction()){
      state = "finding drop off edge";
    }
  } else if (state == "finding drop off edge"){
    // TIMER BASED
    setMotors(200,200);
    delay(3900);
    setMotors(0,0);
    state="release block";

  } else if (state == "release block"){
    // claw drops off block
    openClaw();
    blocksReturned = blocksReturned + 1;
    state = "reverse to home";

  } else if (state=="reverse to home") {
    setMotors(-200,-200);
    delay(2500);
    state="find home edge";
  } else if (state == "find home edge"){
    // find the edge of home square on way back
    if (foundJunction()){
      setMotors(0,0);
      delay(500);
      state = "line follow to j0";
    }
  } else if (state=="line follow to j0"){
    dist = 500000;
      // two different line following processes depending on magnetic/non magnetic - decided to hard code
    if (!magnetic){
      setMotors(-150,150);
      delay(ninetyTurn);
      //find the corner
      while (!foundJunction()){
        setLineSensorValues();
        setMotors(200,200);
        updateBlueLED();
      }
      //turn at the corner
      while (getLP()==2){
        updateBlueLED();
        setLineSensorValues();
        setMotors(-150,150);
      }
      setMotors(200,200);
      delay(500);
      // find node 0
      while (!foundJunction()){
        updateBlueLED();
        setLineSensorValues();
        int lp=getLP();
        // if left of line
        if (lp==-1){
          setMotors(240,0);
        // if on line
        } else if (lp==0){
          setMotors(200,200);
        // if right of line
        } else if (lp==1){
          setMotors(0,240);
        }      
      }
      setMotors(150,-150);
      delay(1200);
      //rotate to face north at node 0
      while (getLP()==2){
        updateBlueLED();
        setLineSensorValues();
        setMotors(150,-150);
      }
    } else {
      setMotors(150,-150);
      delay(ninetyTurn);
      //find the corner
      while (!foundJunction()){
        setLineSensorValues();
        updateBlueLED();
        setMotors(200,200);
      }
      //turn at the corner
      while (getLP()==2){
        updateBlueLED();
        setLineSensorValues();
        setMotors(150,-150);
      }
      setMotors(200,200);
      delay(500);
      //find node 0
      while (!foundJunction()){
        setLineSensorValues();
        updateBlueLED();
        int lp=getLP();
        // if left of line
        if (lp==-1){
          setMotors(240,0);
        // if on line
        } else if (lp==0){
          setMotors(200,200);
        // if right of line
        } else if (lp==1){
          setMotors(0,240);
        // if not on line -> stop
        }      
      }
      setMotors(-150,150);
      delay(1200);
      //rotate to face north at node 0
      while (getLP()==2){
        updateBlueLED();
        setLineSensorValues();
        setMotors(-150,150);
      }
    }
    // need to return to middle of home square and wait after delivering one block
    if (blocksReturned==1){
      startTimerStraighten = millis();
      //line follow briefly out of node 0
      while (millis()-startTimerStraighten<500){
        updateBlueLED();
        setLineSensorValues();
        int lp=getLP();
        // if left of line
        if (lp==-1){
          setMotors(240,0);
        // if on line
        } else if (lp==0){
          setMotors(200,200);
        // if right of line
        } else if (lp==1){
          setMotors(0,240);
        // if not on line -> stop
        }
      }
      //now that straightened, reverse into home
      setMotors(-200,-200);
      delay(1200); 
      setMotors(0,0);
      digitalWrite(blueLED, HIGH);
      delay(5000);
      state="start home";
      direction="north";
      foundTJ=false;
      turning=false;
      leavingJ=false;
      blockFound=false;
    } else {
      // find other blocks
      resetToJState();
    }
  }
}



