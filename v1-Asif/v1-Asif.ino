#include <Servo.h>
#include "Stepper.h"

#define STEPS  32
int  steps, i;
double maxdet = 0.0;
int servopos = 60, anglemax;

Servo myservo;
Stepper stepper(STEPS, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600);
  myservo.attach(5);
  //constrain(servopos, 50, 120);
  myservo.write(60);
  delay(500);
}

void loop() {
  int Rdet = analogRead(A0);
  int Ldet = analogRead(A1);

  if ((Rdet - Ldet > 100) || (Ldet - Rdet > 100)) {
    do {
      Rdet = analogRead(A0);
      Ldet = analogRead(A1);
      if (Rdet - Ldet > 100) {
        rotateCCW();
      }
      else if (Ldet - Rdet > 100) {
        rotateCW();
      }
    } while ((Rdet - Ldet > 100) || (Ldet - Rdet > 100));

    maxdet = max(Rdet, Ldet);
    servopos = 30;
    myservo.write(servopos);
    for (i = 1; i < 14; i++) {
      Rdet = analogRead(A0);
      Ldet = analogRead(A1);
      servopos = servopos + 5;
      myservo.write(servopos);
      if ((Rdet  > maxdet) || (Ldet > maxdet)) {
        maxdet = max(Rdet, Ldet);
        anglemax = servopos;
      }
      delay(150);
    }
    myservo.write(anglemax);

  }

  delay(300);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);

  Serial.print("Rdet : ");
  Serial.print(Rdet);
  Serial.print("  Ldet : ");
  Serial.print(Ldet);
  Serial.print("  maxangle : ");
  Serial.println(servopos);
}

int rotateCCW() {
  stepper.setSpeed(500);
  steps  =  50;
  stepper.step(steps);
}
int rotateCW() {
  stepper.setSpeed(500);
  steps  =  -50;
  stepper.step(steps);
}

int rotateUP() {
  servopos = servopos - 5;
  myservo.write(servopos);
}

int rotateDOWN() {
  servopos = servopos + 5;
  myservo.write(servopos);
}
