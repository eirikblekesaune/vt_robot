
//#include "Demo.h"
/*
REMIXED BY: TECHBITAR / HAZIM BITAR
DATE: APRIL 3, 2012
PROJECT: GOduino II TEST
FUNCTION: This sketch is an Arduino 1.0 IDE test program for the GOduino II PCB  board. The GOduino II is an integrated "runtime" Arduino Uno and L293D h-bridge aka motor driver which allows forward and  reverse movement of motors as well as variable speed.  This test code assumes two motors (each  < 600mA in peak current ) are attached to the GOduino II
This sample sketch calls the following three functions with variable motornum = 1 or 2 to   signify Motor 1 and Motor 2.
- motorforward(int motornum)
- motorback(int motornum)
- motorstop(int motornum)
- motorspeed(int motor1, int motor2)
LICENSE: This is shareware but please give credit. 
WEBSITE: http://techbitar.blogspot.com
EMAIL: techbitar AT gmail DOT com
*/
// map motor poles to Arduino pins
#define motor1pole1 A3//L293_1A 
#define motor1pole2 A4//L293_2A
// map L293d motor enable pins to Arduino pins
#define enablePin1 1 //L293En 
#define M1_MAX_SPEED 100
#define motordelay 30
//#define debugmotorsec 3000

void motorstop();
void motorforward();
void motorback();
void motorspeed(int motor1speed);

