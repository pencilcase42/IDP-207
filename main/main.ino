#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>


// MOTORS //
int blocks_returned = 0;
// DC
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

// Ultrasonic Sensor
// #define MAX_RANG  (520)//the max measurement value of the module is 520cm(a little bit longer than effective max range)
// #define ADC_SOLUTION  (1023.0)//ADC accuracy of Arduino UNO is 10bit

// Time of Flight Sensor
uint16_t dist;
VL53L0X sensor;

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
const int ninetyTurn = 1320;  // time to rotate ninety seconds
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

// Block
bool blockFound = false;
bool magnetic = false;

void calibrateClawHeight(){
  // raise until green button pressed
  clawm->run(BACKWARD);
  clawm->setSpeed(100);
  while (digitalRead(greenButton)==LOW) {
    continue;
  }
  Serial.println("INFO: Green pressed, stop raising...");
  clawm->setSpeed(0);
  delay(1000);
  // lower until green button pressed
  clawm->run(FORWARD);
  clawm->setSpeed(100);
  while (digitalRead(greenButton)==LOW){
    continue;
  }
  Serial.println("INFO: Green pressed, stop lowering...");
  clawm->setSpeed(0);
  // now at bottom
  // raise back up
  raiseClaw();
}


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

  // while(1){
  //   Serial.println("Test open/close claw");
  //   Serial.println(pos);
  //   openClaw();
  //   Serial.println(pos);s
  //   closeClaw();
  // }

  Serial.println("INFO: Calibrating claw height - raise claw, then lower claw");
  openClaw();
  calibrateClawHeight();

  // while (digitalRead(greenButton) == LOW) {
  //   openClaw();
  //   lowerClaw();
  //   closeClaw();
  //   findMagneticType();
  //   Serial.println(magnetic);
  //   raiseClaw();
  // }

}

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
  findMagneticType(); // reset magnetic var.
}

void updateBlueLED(){
  if (lmSpeed==0 && rmSpeed==0){
    digitalWrite(blueLED, LOW);
  } else {
    digitalWrite(blueLED, (millis() / 500) % 2);
  }
}

void loop() {

  if (blockFound){
    dist = 500000;
  } else{
    setDistanceValue();

  }

  // Output current state
  if (previousState!=state) {
    Serial.print("STATE: ");
    Serial.println(state);
    previousState=state;
  }

  setLineSensorValues();

  // Kill/reset button (kill first hit, reset second hit)
  if (digitalRead(greenButton) == HIGH){
    if (state=="kill") {
      Serial.println("INFO: button hit - 2=reset");
      resetToHomeState();
      delay(2000);
    } else {
      state="kill";
      Serial.println("INFO: green button hit - 1=kill");
      delay(1000);
    }
    // resetToHomeState();
    // delay(2000);
    // Serial.println("button hit");
  }

  updateBlueLED();

  if (state=="kill"){
    // stop
    setMotors(0,0);

  } else if (state=="lost"){
    // random search algorithm
    setMotors(0,0);

  } else if (state =="line"){
    if (foundJunction()){
      setMotors(0,0);
      // delay(1000);
      state="junction";
    }else{
      // determine line position
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
      } else if (lp==2){
        // T JUNCTION
        state="TJ";  
      }
    }
    if ((dist<75) && !blockFound) {
      setMotors(0,0);
      blockFound=true;
      // TO TUNE
      Serial.print("INFO: Time of Flight=");
      Serial.println(dist);
      state = "block and pickup";
    }
  } else if (state=="block and pickup"){
    // straighten
    Serial.println("INFO: Straightening at block");
    // setLineSensorValues();
    // while (getLP()==-1){
    //   setMotors(240,0);
    //   setLineSensorValues();
    // }
    // while (getLP()==1){
    //   setMotors(0,240);
    //   setLineSensorValues();
    // }

    // pick up
    Serial.println("INFO: Picking Up block");
    openClaw();
    lowerClaw();
    closeClaw();
    findMagneticType();

    // show detection, raise claw while doing this
    turnOnMagneticLED();    
    raiseClaw();  // includes 5s delay
    turnOffMagneticLED();

    // return to line state
    state="line";

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
    } else if (millis()-startOutJTimer<700){
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
    if (!foundJunction()){
      setMotors(150,150);
    } else if (foundJunction()){
      state = "junction";
    }
  } else if (state == "go middle of home"){
    // get out of j0 and go to middle of home
    setMotors(200,200);
    delay(900);
    state = "turn in home";
  } else if (state=="turn in home") {
    // check if magnetic or not, but for now will use boolean
    // initiate turn
    if (magnetic){
      setMotors(150,-150);
    } else {
      setMotors(-150,150);
    }
    //delay for 90 degree turn 
    delay(ninetyTurn);
    state = "move to edge";
  } else if (state=="move to edge"){
    // find edge of home square and straighten
    setMotors(200,200);
    if (foundJunction()){
      // straightenRear();
      state = "leaving home";
    }
  } else if (state == "leaving home"){
    // leave home
    setMotors(200,200);
    // get out junction
    delay(1000);
    state = "finding drop off edge";
  } else if (state == "finding drop off edge"){

    // find the edge of the square and stop
    if(foundJunction()){
      setMotors(0,0);
      state = "release block";
    }

  } else if (state == "release block"){
    // claw drops off block
    lowerClaw();
    openClaw();
    raiseClaw();
    blockFound = false;
    state = "turn and leave for home";
    blocks_returned = blocks_returned +1;
  } else if (state == "turn and leave for home"){
    //do 180 turn to go back
    if (magnetic){
      setMotors(150,-150);
    } else {
      setMotors(-150,150);
    }
    delay(ninetyTurn*2.1);
    setMotors(200,200);
    delay(1000); // get out of drop off junction
    state = "find home edge";
  } else if (state == "find home edge"){
    // find the edge of home square and straighten
    if (foundJunction()){
      // straightenRear(); 
      state = "go middle home";
    }
  } else if (state == "go middle home"){
    // go to centre of home
    setMotors(200,200);
    delay(850);
    //rotate back towards home junction, left or right turn, initiate rotation
    if (magnetic){
      setMotors(-150,150);
    } else {
      setMotors(150,-150);
    }
    delay(ninetyTurn);
    setMotors(0,0);
    digitalWrite(blueLED, HIGH);
    delay(5000);
    resetToHomeState();
  }
}



