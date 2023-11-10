#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(1);
Adafruit_DCMotor *rm = AFMS.getMotor(2);

// motor speeds (0-255)
int lmSpeed=0;
int rmSpeed=0;

// led pin#
int led = 7;

// button pin#
int button = 8;

// light sensor pins (front and rear (r))
int leftPin = 2;
int rightPin = 6;
int rLeftPin = 4;
int rRightPin = 3;

// light sensor values (1 is white, 0 is black)
int valLeft, valRight, valRLeft, valRRight;


// set the motor speeds (left motor speed, right motor speed) (-255 -> +255)
void setMotors(int newLeft, int newRight){
  setLM(newLeft)
  setRM(newRight)
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
  Serial.println("Light sensors: init..")
  pinMode(leftPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(rLeftPin, INPUT);
  pinMode(rRightPin, INPUT);

  // init led
  Serial.println("LED: init...")
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // init button
  //Serial.println("Button: init...")
  pinMode(button, OUTPUT);


  // TESTING //
  motorTest()


}


void loop() {

  // find and set line sensor values
  setLineSensorValues();

  // check if at junction (0 means no junction, on line)
  if (getJT()==0) {
    Serial.println("Status: JUNCTION")
    setMotors(0,0); // stop
    // to do
  // else on line
  } else {

    // determine line position
    int lp=getLP();
    Serial.println("Status: LINE - "+str(lp));
    Serial.println("Sensors: (left,right)="+str(valLeft)+str(valRight));
    // if left of line
    if (lp==-1){
      setMotors(200,100);
    // if on line
    } else if (lp==0){
      setMotors(200,200);
    // if right of line
    } else if (lp==1){
      setMotors(100,200);
    // if not on line -> stop
    } else if (lp==2){
      setMotors(0,0);
    }

  }

}
