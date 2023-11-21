#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(1);
Adafruit_DCMotor *rm = AFMS.getMotor(2);


#define MAX_RANG  (520)//the max measurement value of the module is 520cm(a little bit longer than effective max range)
#define ADC_SOLUTION  (1023.0)//ADC accuracy of Arduino UNO is 10bit

//ultrasonic sensor
int sensityPin = A0;
float dist, sensity;

// magnetic sensor
bool magnetic = true;

//rotating 
const int ninetyturn = 1000;
String turningDirection;
// motor speeds (0-255)
int lmSpeed=0;
int rmSpeed=0;

// led pin#
const int blueLED = 6;

// button pin#
const int greenButton = 7;
const int redButton = 8;

// light sensor pins (front and rear (r))
const int rLeftPin = 2;
const int rRightPin = 3;
const int leftPin = 4;
const int rightPin = 5;

// TJ state
unsigned long startTJtimer;
bool foundTJ = false;

unsigned long startOutJTimer;

// robot state
String state;
String direction;
bool turning;//REVIEW

// light sensor values (1 is white, 0 is black)
int valLeft, valRight, valRLeft, valRRight;

//block
bool block_found = false;

void setDistanceValue(){
  sensity = analogRead(sensityPin);
  dist = sensity * MAX_RANG/ADC_SOLUTION;
}


void setup() {

  // SETUP //

  // init serial
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Initialising...");

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
  Serial.println("LED: init...");
  pinMode(blueLED, OUTPUT);
  // digitalWrite(blueLED, LOW);

  // init button
  Serial.println("Button: init...");
  pinMode(greenButton, INPUT);
  pinMode(redButton, INPUT);

  // init robot state
  state="line";
  direction="north";
  turning=false;

  // TESTING //
  //motorTest();

}


void loop() {

  setDistanceValue();
  Serial.println(dist);
  setLineSensorValues();

  // //printing values for testing
  // Serial.print("State: ");
  // Serial.print(state);
  // Serial.print(", Millis: ");
  // Serial.print(millis());
  // Serial.print(", startTJtimer: ");
  // Serial.print(startTJtimer);
  // Serial.print(", turning: ");
  // Serial.print(turningLeft);
  // Serial.print(", foundTJ: ");
  // Serial.println(foundTJ);
  // Serial.println();

  if (digitalRead(greenButton) == HIGH){
    Serial.println("reset button hit");
    state = "line ";
    setMotors(0,0);
    delay(1000);
    foundTJ=false;
  }

  if (digitalRead(redButton) == HIGH){
    Serial.println("emergency stop hit - kill robot");
    state = "kill";
  }

  if (lmSpeed==0 && rmSpeed==0){
    digitalWrite(blueLED, LOW);
  } else {
    digitalWrite(blueLED, (millis() / 500) % 2);
  }

  if (state=="kill"){
    // stop
    setMotors(0,0);

  } else if (state=="lost"){
    // random search algorithm
    setMotors(0,0);

  } else if (state =="line"){

    if (foundJunction()){
      setMotors(0,0);
      delay(1000);
      state = "junction";
    }else{
      // determine line position
      int lp=getLP();
      //String message = "Status: LINE -" + lp;
      Serial.print("Line Position:");
      Serial.println(lp);
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
        Serial.println("SET STATE TO TJdvdvvdvdvdvvdvdvdvdvdvdvvdvdvvdvdvdvvd");
        state = "TJ";  
      }
      //ULTRASONIC
      // checking if close to block, will ultimately call brain
      if (dist < 5){
        Serial.println("Detected block");
        setMotors(0,0);
        state = "block";
      }
    }
  } else if (state == "TJ"){
    if (foundTJ==false){
      Serial.println("START looking for TJ");
      startTJtimer = millis();   
      foundTJ=true;      
    }
    if (foundJunction()) {
      // found, leave loop
        Serial.println("found TJ");
        foundTJ=false;
        state="junction";
    } else if ((millis() - startTJtimer) < 3000) {
      // not found, but yes keep looking
      Serial.println("looking for TJ");
      setMotors(150,150); 
    } else {
      Serial.println("ABORT looking for TJ");
      // not found, lost mode
      foundTJ = false;
      state="lost";
    }
  } else if (state == "junction"){
    if (!turning) {
      turningDirection = junctionReached();
      if (turningDirection == "forward"){
        state = "leaving junction";
      } else if (turningDirection == "right"){
          setMotors(150,-150);
          delay(500);
      } else if (turningDirection == "left"){
          setMotors(-150,150);
          delay(500);
      } else if (turningDirection == "back"){
        setMotors(-150,150);
        delay(ninetyturn*1.3);
      }
      turning = true;
    } else if (turning) {
      if((valLeft == 0) || (valRight == 0)){
        // continue
        Serial.println("still rotating");
      } else{
        // left line found
        Serial.println("line found, leaving now");
        // stop turning
        turning = false;
        // leave junction mode
        state = "leaving junction";
      }
    }
  } else if (state == "leaving junction"){
    startOutJTimer = millis();
    while (millis()-startOutJTimer<800){
      Serial.println("line following OUT OF JUNCTION");
      setLineSensorValues();
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
    }
    Serial.println("out of junction");
    state = "line"; // once left junction go back to line state
  } else if (state == "block"){
    // pick up block then go to junction, then call brain, then to start then to red/green
    Serial.println("Enter block state");
  } else if (state == "start home"){
    if (!foundJunction()){
      setMotors(150,150);
    } else if (foundJunction()){
      state = "leaving junction";
    }
  } else if (state == "go to drop off - 1"){
    // get out of junction and go to middle of square
    setMotors(200,200);
    delay(2000);
    state == "go to drop off - 2";

  } else if (state=="go to drop off - 2") {
    // read magnetic sensor and check if magnetic or not, but for now will use boolean
    // initiate turn
    if (magnetic){
      setMotors(-150,150);
    } else {
      setMotors(150,-150);
    }
    //delay for 90 degree turn 
    delay(ninetyturn);
    state = "go to drop off - 3";
  } else if (state=="go to drop off - 3"){

      // find edge of home square and straighten
      setMotors(200,200);
      if (foundJunction()){
        // straighten
        while ((valRLeft == 0) && (valRRight == 1)){
            setMotors(100,0);
        }
        while ((valRLeft == 1) && (valRRight == 0)){
            setMotors(0,100);
        }
        setMotors(0,0);
        state="go to drop off - 4";
      }
  } else if (state=="go to drop off - 4"){

      // leave home
      setMotors(200,200);
      // get out junction
      delay(1000);
      // find the edge of the square and stop
      while(!foundJunction()){
        continue;
      }
      setMotors(0,0);

      // drop off the block
      // TO DO
      delay(1000);


      //do 180 turn to go back
      setMotors(150,-150);
      delay(ninetyturn*2);
      setMotors(200,200);
      delay(1000); // get out of drop off junction

      // find the edge of home square and straighten
      while (!foundJunction()){
        continue;     
      }
      while ((valRLeft == 0) && (valRRight == 1)){
        setMotors(100,0);
      }
      while ((valRLeft == 1) && (valRRight == 0)){
        setMotors(0,100);
      }

      // go to centre of home
      setMotors(200,200);
      delay(2000);
    
    //rotate back towards home junction, left or right turn, initiate rotation
     if (magnetic){
        setMotors(150,-150);
      } else {
        setMotors(-150,150);
      }
      delay(ninetyturn);
      setMotors(200,200);

      while (!foundJunction()){
        continue;     
      }
      //stop when reached first junction
      setMotors(0,0);
      delay(1000);
  }


  
}



