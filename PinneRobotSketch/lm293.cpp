// MOTOR FUNCTIONS
#include <Arduino.h>
#include "lm293.h"

// MOTOR FUNCTIONS

void motorstop(){
  delay(motordelay);
  digitalWrite(motor1pole1, LOW);
  digitalWrite(motor1pole2, LOW);
  delay(motordelay);
}

void motorforward(){
  digitalWrite(motor1pole1, HIGH);
  digitalWrite(motor1pole2, LOW);
  delay(motordelay);
}

void motorback(){
  digitalWrite(motor1pole1, LOW);
  digitalWrite(motor1pole2, HIGH);
  delay(motordelay);
}

void motorspeed(int motor1speed) {
  if (motor1speed > M1_MAX_SPEED ) motor1speed = M1_MAX_SPEED; // limit top speed
  if (motor1speed < 0) motor1speed = 0; // keep motor above 0
   analogWrite(enablePin1, motor1speed);
}
