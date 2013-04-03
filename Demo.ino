//#include "DualVNH5019MotorShield.h"
#include "motor_controll.h"
#include <avr/interrupt.h>
#include <PinChangeInt.h>

motorControll motorL (0,A1,A3,A5);

struct{
    bool    MOTOR_TO_POSITION ;
    bool    MOTOR_TO_ZERO_POSITION ;
    bool    CARE_ABOUT_SET_POSITION ;
}STATE;

//kSTATE:MOTOR_TO_POSITION = false;
// const int stopSwLeft = A3 ;
// const int stopSwRight = A2 ;
// const int right2hal = A4;
// const int left2hal = A5;
// int rightSpeed = 100;

// void stopIfFault()
// {
//   if (md.getM1Fault())
//     {
//       Serial.println("M1 fault");
//       while(1);
//     }
//   if (md.getM2Fault())
//     {
//       Serial.println("M2 fault");
//       while(1); // enable later
//     }
// }

// void stopLeft(){
//     md.setM2Brake(1);
//     while(digitalRead(stopSwLeft) == 0 ){
// 	md.setM2Speed(100);
//     }
//     md.setM2Brake(1);
//     leftMotor.position = 0; // resets motor position
// }  
// void stopRight(){
//   md.setM1Brake(1);
//     while(digitalRead(stopSwRight) == 0 ){
// 	md.setM1Speed(-100);
//     }
//     md.setM1Brake(1);
// }  

// Void Left()
// {

//     motorL.position();
//     if(digitalRead(left2hal) == HIGH) {
// 	leftMotor.position++;
// 	leftMotor.direction = DOWN ; 

//     }
//     else {
// 	leftMotor.position--;
// 	leftMotor.direction = UP ; 
//     }
    
//     if((digitalRead(stopSwLeft)) == 0){
// 	leftMotor.direction = STOP;
// 	stopLeft();
//     }
// }


int i = 0 ;
 void right()
{
    i++;
    Serial.print(i);
    Serial.print(" ");
    //    motorL.position();
}

void setup()
{
  Serial.begin(115200);
  //  md.init(); // set INA1 to INA5

  // pinMode(stopSwLeft, INPUT);   
  // pinMode(stopSwRight, INPUT);   
  // pinMode(left2hal, INPUT_PULLUP); 
  // pinMode(right2hal, INPUT_PULLUP); 
  // attachInterrupt(0, left, RISING);
 
  motorL.initMotorShield();
  motorL.init();

  uint8_t inter1 = 2;
  //attachInterrupt(0, right, RISING);
  pinMode(PIN1, INPUT); digitalWrite(PIN1, HIGH);
  PCintPort::attachInterrupt(2, &right, CHANGE);
  //  void PCintPort::attachInterrupt(inter1, motorL.position(), int mode)
//


STATE.MOTOR_TO_POSITION= true;

}


#define state 0b001
#define targetPos 0b0010
#define speed 0b1000
#define motorPos 0b0101
#define motorSpeed 0b1100
#define stop 0b0000

typedef struct{
byte isCmd:  1;
byte dataNext : 1;
byte leftOrRight: 1;
byte setOrGet : 1;
byte stateCmd : 4;
}inData ;
inData data;


// void parseStickCmd( byte keyCmd)
// {
//     keyCmd = B10100000; // remove
//     data.isCmd = bitRead(keyCmd, 7);
//     data.dataNext = bitRead(keyCmd, 6);
//     data.leftOrRight = bitRead(keyCmd, 5);
//     data.setOrGet = bitRead(keyCmd, 4);
    
    
//     //    Serial.print(keyCmd, BIN );
//     if( data.isCmd == true )
// 	{
// 	    //	    Serial.print(F("got command"));
// 	    if( data.leftOrRight == true){
// 		;//left
// 	    }
// 	    else {
// 		;//right
// 	    }
// 	}
    
//     if( data.setOrGet == true){
// 	;//setOrGetdatabyte
//     }
    
    
//}
void parseMenu(byte key_command)
{
 
  Serial.print(F("Received command: "));
  Serial.write(key_command);
  Serial.println(F(" "));

  // if(key_command == 'r')  { // set speed
  //   int i = 50;
  //   if(Serial.read())  {
  //     i =  Serial.parseInt();
  //   }
  //   md.setM1Speed(i);
  //   Serial.println(F("writespeed "));
  // }
  // if(key_command == 'l')  { // set speed
  //   int i = 100 ;
  //   if(Serial.read())  {
  //     i =  Serial.parseInt();
  //   }
  //   md.setM2Speed(i);
  // }
  // if(key_command == 'o')  { // set speed
  //     leftMotor.fart = 100;
  // }
  if(key_command == 'p')  { // print left and right motor count
      Serial.println(("leftMotorCount : "));
      int i = motorL.getPos();
      Serial.print(i);
      Serial.println(("leftMotortarget : "));
      int g   = motorL.getTarget();
      Serial.print(g);
      // Serial.println(("leftMotorsetPos : "));
      // Serial.print(motorL.setPos);
      // Serial.println(("leftMotorSpeed : "));
      // Serial.print(motorL.fart);
      // Serial.println(F("rightMotorCount : "));
      // Serial.println(F("\n "));
  }
  if(key_command == 'm')  {
      int i = 0;
      if(Serial.read()){
	  i = Serial.parseInt();
      }
            motorL.setPos(i);
  }
 
if(key_command == 'l')  {
    int i = 0;
    if(Serial.read()){
	i = Serial.parseInt();
    }
    motorL.setSpeed(i);

 }
 if(key_command == 'n'){
     if(STATE.MOTOR_TO_POSITION== true){
	 Serial.print("Position False");
	 STATE.MOTOR_TO_POSITION= false;
     }else{
	 STATE.MOTOR_TO_POSITION= true;
	 Serial.print("Position True");
     }
	
 }
 if(key_command == 's'){
     if( STATE.MOTOR_TO_ZERO_POSITION == true){
	 STATE.MOTOR_TO_ZERO_POSITION= false;
     }else{
	 STATE.MOTOR_TO_ZERO_POSITION= true;
     }

 }
} 

// void checkMotorPos(motorData& motor){
//     if (motor.setPos < motor.position){
// 	motor.direction = UP ; 
//     }
//     else if (motor.setPos > motor.position) {
// 	motor.direction = DOWN ;}
//     //    else if (motor.setPos == motor.position) {
//     //	motor.direction = STOP ; 
//     //    }
// }

// void motorToZeroPosition (motorData& motor){
//       md.setM2Speed(-100);
//       while(leftMotor.position != 0 ){ 
// 	  Serial.println(F("got to leftZero "));
// 	  if(leftMotor.direction == STOP ){ 	  
// 	      Serial.println(F("got to leftZero "));
// 	      break;
// 	  }
//       }

//       leftMotor.direction = DOWN;
// }

// void goToPos(motorData& motor){
//     checkMotorPos( motor);

// }

void loop()
{

  if(Serial.available()) {
    parseMenu(Serial.read());
  }
  //  checkMotorPos(leftMotor );
  //  stopMotorWhenPositioned(leftMotor);

    if (STATE.MOTOR_TO_POSITION == true){
      motorL.goToPos();
    }
  if (STATE.MOTOR_TO_ZERO_POSITION){ // update for both motors
      //      motorToZeroPosition(leftMotor);// locks program for input
      motorL.resetPos();
  }
}
