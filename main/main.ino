#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *lm = AFMS.getMotor(1);
Adafruit_DCMotor *rm = AFMS.getMotor(2);

int lmSpeed=0;
int rmSpeed=0;


int getLP(){  
  //return -1, 0, 1

  int arrayNum[3] = {-1,0,1};
  int RandIndex = rand() % 2; // random between zero and four
  Serial.print(arrayNum[RandIndex]);
  return arrayNum[RandIndex];
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

  Serial.println("Motot testtt");
  forwards();
  delay(2000);
  halt();
  delay(2000);
  backwards();
  delay(2000);
  halt();

}


void loop() {
 delay(2000);
 int lp=getLP();
 if (lp==-1){
  setLM(50);
  setRM(40);
 } else if (lp==0){
  setLM(40);
  setRM(40);
 } else if (lp==1){
  setLM(40);
  setRM(50);
 }
}
