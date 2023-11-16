#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(1);
Adafruit_DCMotor *rm = AFMS.getMotor(2);

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



// robot state
String state;
String direction;
bool turningLeft;//REVIEW


// light sensor values (1 is white, 0 is black)
int valLeft, valRight, valRLeft, valRRight;


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
  direction="North";
  turningLeft=false;

  // TESTING //
  //motorTest();

}


void loop() {

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
    state = "line";
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
        setMotors(200,180);
      // if right of line
      } else if (lp==1){
        setMotors(0,240);
      // if not on line -> stop
      } else if (lp==2){
        // T JUNCTION
        Serial.println("SET STATE TO TJdvdvvdvdvdvvdvdvdvdvdvdvvdvdvvdvdvdvvd");
        state = "TJ";
        
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
        setMotors(0,0);
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
    if (!turningLeft) {
      turningLeft = true;
      setMotors(150,-150);
      delay(500);
    } else if (turningLeft) {
      if((valLeft == 0) || (valRight == 0)){
        // continue
        Serial.println("still rotating left");
        setMotors(100,-100);
      } else{
        // left line found
        Serial.println("left line found");
        setMotors(0,0);
        delay(1000);
        Serial.println("have stopped");
        // stop turning
        turningLeft = false;
        // leave junction mode
        state = "leaving junction";
      }
    }
  } else if (state == "leaving junction"){
    setMotors(150,150);
    delay(500);
    Serial.println("out of junction");
    state = "line";
  }
}



