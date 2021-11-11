#include <Stepper.h>
#include <Servo.h>

// Photoresistors Analog Pins and Variables
int EAST_LDR_PIN = 0;
int WEST_LDR_PIN = 1;
int eastLDR = 0;
int westLDR = 0;
int deltaLDR = 0;
int averageLDR = 0;
const int moveCutoff = 30;

// Servo Pin and Variables
const int SERVO = 9;
int servoAngle = 0; // Current servo angle
int nextAngle = 90;
int prevAngle = 0;
int prevLDR = 0;
int nextLDR = 0;
int tempAngle = 0;
int tempLDR = 0;


// Maunual Buttons to turn stepper PINS
int CCW_BUTTON = 2;
int CW_BUTTON = 3;

// Motor Pins and Variables
const int IN1 = 5;
const int IN2 = 6;
const int IN3 = 10;
const int IN4 = 11;
const int stepsPerRevolution = 2048;
int stepperSpeed = 0;
int dir = 0; // Direction to spin stepper for manual control
int stepperPOS = stepsPerRevolution * 0.375; // Pos repersenting stepper(limited to 0 to 180)

int operationMode = 0;

Stepper myStepper = Stepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Servo myServo = Servo();

void setup() {
  pinMode(CW_BUTTON, INPUT_PULLUP);
  pinMode(CCW_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CW_BUTTON), turnCW, FALLING);
  attachInterrupt(digitalPinToInterrupt(CCW_BUTTON), turnCCW, FALLING);

  myStepper.setSpeed(3);
  myServo.attach(9);
  Serial.begin(9600);
  myServo.write(00);
}

void findNextAngle () {
  //Serial.print(nextAngle);
  //Serial.print(",");
  myServo.write(nextAngle);
  delay(500);
  nextLDR = getAvgLDR();
  //Serial.println(nextLDR);
  if (nextLDR > prevLDR) {
    tempAngle = nextAngle;
    tempLDR = nextLDR;
  } 
  else {
    tempAngle = prevAngle;
    tempLDR = prevLDR;
  }
  nextAngle = (nextAngle + prevAngle)/2;
  prevAngle = tempAngle;
  prevLDR = tempLDR;
}

double getAvgLDR() {
  eastLDR = analogRead(EAST_LDR_PIN);
  westLDR = analogRead(WEST_LDR_PIN);
  averageLDR = (eastLDR + westLDR) / 2;
  return averageLDR;
}

double getDeltaLDR (){
  eastLDR = analogRead(EAST_LDR_PIN);
  westLDR = analogRead(WEST_LDR_PIN);
  deltaLDR = eastLDR - westLDR;
  return deltaLDR;
}


void turnCW() {
  dir = -1;
}

void turnCCW() {
  dir = 1;
}

void loop() {
  deltaLDR = getDeltaLDR();

  if (abs(deltaLDR) > moveCutoff)
    operationMode = 0;
  else if(operationMode != 2)
    operationMode = 1;

  if (dir != 0) {
    myStepper.step(dir * stepsPerRevolution / 64);
    dir = 0;
  }

  Serial.print(eastLDR);
  Serial.print(",");
  Serial.print(westLDR);
  Serial.print(",");
  Serial.print(deltaLDR);
  Serial.print(",");
  Serial.print(operationMode);
  Serial.print(",");
  Serial.println(stepperPOS);

  if (operationMode == 0) {
    if (deltaLDR < -moveCutoff && stepperPOS < stepsPerRevolution * 0.75) {
      stepperPOS += stepsPerRevolution / 64;
      if (stepperPOS >= stepsPerRevolution * 0.75) {
        stepperPOS = stepsPerRevolution / 2;
      } else {
        myStepper.step(stepsPerRevolution / 64);
      }
      myStepper.step(stepsPerRevolution / 64);
    } else if (deltaLDR > moveCutoff && stepperPOS > 0) {
      stepperPOS -= stepsPerRevolution / 64;
      if (stepperPOS <= 0) {
        stepperPOS = 0;
      } else {
        myStepper.step(-stepsPerRevolution / 64);
      }
    }
  } else if (operationMode == 1) {
    prevAngle = 0;
    nextAngle = 60;
    myServo.write(prevAngle);
    delay(200);
    prevLDR = getAvgLDR();
    
    while (abs(prevAngle - nextAngle) > 2 && abs(getDeltaLDR()) < moveCutoff) {
      findNextAngle();
    }
    operationMode = 2;
  }
  delay(50);
}
