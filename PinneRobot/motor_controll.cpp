#include "motor_controll.h"

motorControll::motorControll(){
    _stopSw = A3;// stopSw;
    _sensor = A5;// sensor;
}





int p;
void encoder(){
    //    Serial.print("interrupt");
    

}



motorControll::motorControll(int interrupt, int  motor, int stopSw, int sensor){
    
        _stopSw =  stopSw;
	_sensor =  sensor;
	attachInterrupt(interrupt, encoder, RISING);
}
void motorControll::init(){
    pinMode(_stopSw, INPUT);   //does this work?
    pinMode(_sensor, INPUT_PULLUP);
    motor.setPos = 0; //current motor position
    motor.setSpeed = 0;
} 

void motorControll::initMotorShield(){
    DualVNH5019MotorShield::init();
}

void motorControll::position(){
    i=  getPos();
    Serial.print(i);
    if(digitalRead(_sensor) == HIGH) {
	motor.curPos++;
	motor.direction = DOWN ; 

    }
    else {
	motor.curPos--;
	motor.direction = UP ; 
    }
    
    if((digitalRead(_stopSw)) == 0){
	motor.direction = STOP;
	Serial.println(F("STOP MOTOR! "));
	stopMotor();
    }
}

int motorControll::getPos(){
    return motor.curPos;
}
int motorControll::getTarget(){
    return motor.setPos;
}

void motorControll::setPos(int _setPos){
    motor.setPos = _setPos;
}

void motorControll::setSpeed(int setSpeed){
    motor.setSpeed = setSpeed;
    motorShield.setM2Speed(motor.setSpeed);
}

void motorControll::goToPos()
{
    if( motor.direction == DOWN && (motor.curPos > motor.setPos)){
	Serial.print("Goind down");
	stopMotor();
    }
    if ( motor.direction == UP && ( motor.curPos < motor.setPos)){
	Serial.print("going up");
	stopMotor();
    }
    if (motor.direction == STOP){ 
	//Serial.print("STOP POS");
    }
}


//    motorShield.setM2Speed(motor.setSpeed);

void motorControll::resetPos(){
    motorShield.setM2Speed(-100);
      while(_stopSw == 1 ){
	  if(motor.direction == STOP ){ 	  
	      Serial.println(F("got 0 "));
	      motor.curPos = 0; // resets motorPos 
	      break;
	  }
      }
      motor.direction = DOWN;
}


void motorControll::stopMotor(){
    motorShield.setM2Brake(1);
    while(digitalRead(_stopSw) == 0 ){ //when switch works
 	motorShield.setM2Speed(-100);
    }
    motorShield.setM2Brake(1);
    motor.direction = STOP;
    Serial.print("ZERO");
}  
