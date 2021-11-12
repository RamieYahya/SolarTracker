#include <Servo.h>
#include "Stepper.h"

#define STEPS  32 //steps for internal shaft revolution

int  steps, i, servopos = 60, anglemax, deltadet = 100, steppersteps = 50;  //det used in a variable means detector/detected
double maxdet = 0.0;

Servo myservo;
Stepper stepper(STEPS, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600);
  myservo.attach(5);
  myservo.write(60);  //at this angle, detector can see light at any height
  delay(500);
}

void loop() {
  int Rdet = analogRead(A0);  //pull readings from photoresistors, R & L = right/left
  int Ldet = analogRead(A1);

  if ((Rdet - Ldet > deltadet) || (Ldet - Rdet > deltadet)) { //checks if the right or left photoresistor is exposed to more light
    do {
      Rdet = analogRead(A0);
      Ldet = analogRead(A1);
      if (Rdet - Ldet > deltadet) {  //if the right side of the detector is brighter, rotate azimuth in that direction
        rotateCCW();
      }
      else if (Ldet - Rdet > deltadet) { //if the left side of the detector is brighter, rotate azimuth in that direction
        rotateCW();
      }
    } while ((Rdet - Ldet > deltadet) || (Ldet - Rdet > deltadet)); //keep rotating along the azimuth until reading between sensors is balanced

    maxdet = max(Rdet, Ldet); //current max detected reading between sensors
    servopos = 30;  //highest altitude for detector
    myservo.write(servopos);
    for (i = 1; i < 14; i++) {
      Rdet = analogRead(A0);
      Ldet = analogRead(A1);
      servopos = servopos + 5;  //cycles through set altitude angles until it reaches the lowest altitude for the detector
      myservo.write(servopos);
      if ((Rdet  > maxdet) || (Ldet > maxdet)) {  //checks if the light on either sensor is brighter than the previous max brightness
        maxdet = max(Rdet, Ldet); //sets the new max brightness value
        anglemax = servopos;  //saves the angle at which max brightness was fount
      }
      delay(150); //ideally, there is very little delay, but this can lead to an unstable system 
    }
    myservo.write(anglemax);  //sets the altitude angle to the max brightness angle

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
  steps  =  steppersteps;
  stepper.step(steps);
}
int rotateCW() {
  stepper.setSpeed(500);
  steps  =  -steppersteps;
  stepper.step(steps);
}
