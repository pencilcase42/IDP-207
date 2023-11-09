#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(1);
Adafruit_DCMotor *rm = AFMS.getMotor(2);

int lmSpeed=0;
int rmSpeed=0;

int led = 7;

// light sensor pins
int forwardsp = 2;
int backsp = 6;
int leftsp = 4;
int rightsp = 3;

int valLeft, valRight, valForward, valBack; // light sensor values: 1 is white, 0 is black

void setLineSensorValues(){
  valLeft = digitalRead(leftsp);
  valRight = digitalRead(rightsp);
  valForward = digitalRead(forwardsp);
  valBack = digitalRead(backsp);
}



void setLM(int newSpeed){
  if (newSpeed==lmSpeed){
    return;
  }
  else{
      lmSpeed=newSpeed;
      if (lmSpeed>0){
        lm->run(FORWARD);
        lm->setSpeed(lmSpeed);
      } else if (lmSpeed==0){
        lm->run(RELEASE);
      } else if (lmSpeed<0){
        lm->run(BACKWARD);
        lm->setSpeed(-lmSpeed);
      }
  }
}

void setRM(int newSpeed){
  if (newSpeed==rmSpeed){
    return;
  }
  else{
    rmSpeed=newSpeed;
    if (rmSpeed>0) {
      rm->run(FORWARD);
      rm->setSpeed(rmSpeed);
    } else if (rmSpeed==0){
      rm->run(RELEASE);
    } else if (rmSpeed<0){
      rm->run(BACKWARD);
      rm->setSpeed(-rmSpeed);
    }
  }
}


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Initialise Adafruit Motorshield v2....");

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  Serial.println("Motor Shield found.");

  pinMode(forwardsp, INPUT);
  pinMode(backsp, INPUT);
  pinMode(leftsp, INPUT);
  pinMode(rightsp, INPUT);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

}


void loop() {

/*  Serial.println("Motot testtt");
  forwards();
  delay(2000);
  halt();
  delay(2000);
  backwards();
  delay(2000);
  halt();
  delay(2000);
  rotateRight();
  delay(2000);
  rotateLeft();
  delay(2000);
  halt();
*/
setLineSensorValues();
 int lp=getLP();
 Serial.println(valLeft);
 Serial.println(valForward);
 Serial.println(valRight);
 Serial.println(valBack);
  Serial.println("Line position: ");
  Serial.print(lp);

 if (lp==-1){
  setLM(200);
  setRM(100);
 } else if (lp==0){
  setLM(200);
  setRM(200);
 } else if (lp==1){
  setLM(100);
  setRM(200);
 } else if (lp==2){
  setLM(0);
  setRM(0);
 }


}
