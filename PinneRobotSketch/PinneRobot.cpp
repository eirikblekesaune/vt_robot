#include "PinneRobot.h"

//pin connections
const int leftDriverPWM = 9;
const int leftDriverINA = 5;
const int leftDriverINB = 4;
const int leftDriverCS = A0;
const int leftDriverENDIAG = 6;
const int leftMotorEncoderInterruptIndex = 0;// digital pin 2 implicitly
const int leftMotorSTOP = A5;

const int rightDriverPWM = 10;
const int rightDriverINA = 12;
const int rightDriverINB = 8;
const int rightDriverCS = A1;
const int rightDriverENDIAG = 7;
const int rightMotorEncoderInterruptIndex = 1;// digital pin 3 implicitly
const int rightMotorSTOP = 13;

const int rotationDriver1A = A3;
const int rotationDriver2A = A4;
const int rotationDriverPWM = 11;

VNH5019Driver leftDriver(leftDriverINA, leftDriverINB, leftDriverENDIAG, leftDriverCS, leftDriverPWM);
VNH5019Driver rightDriver(rightDriverINA, rightDriverINB, rightDriverENDIAG, rightDriverCS, rightDriverPWM);

PinneMotor leftMotor(leftMotorSTOP, leftMotorEncoderInterruptIndex, &leftDriver);
//PinneMotor rightMotor(rightMotorSTOP, rightMotorEncoderInterruptIndex, &rightDriver);

//
//volatile position_t posM1 = 0;
//volatile position_t posM2 = 0;
//volatile int encoder1TickValue = 1;
//volatile int encoder2TickValue = 1;
//
//void encoder1_ISR()
//{
//  posM1 = posM1 + encoder1TickValue;
//}
//
//void encoder2_ISR()
//{
//  posM2 = posM2 + encoder2TickValue;
//}
//
//void PinneRobot::init()
//{
//  md.init();
//  attachInterrupt(0, encoder1_ISR, CHANGE);
//  attachInterrupt(1, encoder2_ISR, CHANGE);
//  pinMode(M1_HALLA, INPUT);
//  pinMode(M1_HALLB, INPUT);
//  pinMode(M1_STOP, INPUT);
//  pinMode(M2_HALLA, INPUT);
//  pinMode(M2_HALLB, INPUT);
//  pinMode(M2_STOP, INPUT);
//  pinMode(L293_1A, OUTPUT);
//  pinMode(L293_2A, OUTPUT);
//  pinMode(L293_EN, OUTPUT);
//  
//  _initLeftMotor();
//}
//
//void PinneRobot::_initLeftMotor()
//{
//  _leftMotor.speed = SPEED_STOP;
//  _leftMotor.direction = DIRECTION_DOWN;
//  _leftMotor.target = TARGET_NONE;
//  _leftMotor.minPosition = POSITION_ALL_UP;
//  _leftMotor.maxPosition = POSITION_DEFAULT_MAX;
//  _leftMotor.brake = BRAKE_NONE;
//  _leftMotor.stopButtonValue = digitalRead(M1_STOP);
//}
//
//void PinneRobot::checkSensors()
//{
//  int i, val;
//  val = digitalRead(M1_STOP);
//  if( val != _leftMotor.stopButtonValue)
//  {
//    if(val == BUTTON_IN)
//    {
//      _leftMotorStopSensorHit();
//    } else {
//      setLeftMotorBrake(BRAKE_NONE);
//    }
//  }
//}
//
//void PinneRobot::updatePositions()
//{
//  position_t currentPosition;
//  if(_leftMotor.target != TARGET_NONE)
//  {
//    if(getLeftMotorDirection() == DIRECTION_UP)
//    {
//      if(posM1 <= _leftMotor.target)
//      {
//        _leftMotorTargetReached();
//      }
//    } else {//otherwise the direction is DIRECTION_DOWN
//      if(posM1 >= _leftMotor.target)
//      {
//        _leftMotorTargetReached();
//      }
//    }
//  }
//  currentPosition = getLeftMotorCurrentPosition();
//  if(!_leftMotor.blocked)
//    {
//    if(currentPosition <= POSITION_ALL_UP)
//    {
//      _leftMotorSetBlocked(true);
//      _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_MIN_POSITION);
//    }
//    if(currentPosition >= getLeftMotorMaxPosition())
//    {
//      _leftMotorSetBlocked(true);
//      _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_MAX_POSITION);
//    }
//  }
//}
//
//void PinneRobot::setLeftMotorSpeed(speed_t speed)
//{
//  _leftMotor.speed = constrain(speed, SPEED_MIN, SPEED_MAX);
//  if(_leftMotor.speed == 0)
//  {
//    leftMotorStop();
//  } else {
//    _leftMotorCalculateAndSetSpeed();
//  }
//}
//
//speed_t PinneRobot::getLeftMotorSpeed()
//{
//  return _leftMotor.speed;
//}
//
//void PinneRobot::leftMotorStop()
//{
//  _leftMotor.speed = SPEED_STOP;
//  _leftMotorCalculateAndSetSpeed();
//  _notifyStateChange(LEFT_MOTOR_STOPPED);
//}
//
//void PinneRobot::setLeftMotorDirection(direction_t direction)
//{
//  if(direction != _leftMotor.direction)
//  {
//    if(direction == DIRECTION_DOWN)
//    {
//      if(direction != _leftMotor.direction)
//      {
//        _notifyStateChange(LEFT_MOTOR_GOING_DOWN);
//      }
//      _leftMotor.direction = direction;
//      encoder1TickValue = 1;
//    } else if(direction == DIRECTION_UP) {
//          if(direction != _leftMotor.direction)
//      {
//        _notifyStateChange(LEFT_MOTOR_GOING_DOWN);
//      }
//      _leftMotor.direction = direction;
//      encoder1TickValue = -1;
//      _notifyStateChange(LEFT_MOTOR_GOING_UP);
//    }
//  }
//  _leftMotorCalculateAndSetSpeed();
//}
//
//direction_t PinneRobot::getLeftMotorDirection()
//{
//  return _leftMotor.direction;
//}
//
//void PinneRobot::setLeftMotorTargetPosition(position_t position)
//{
//  _leftMotor.target = constrain(position, max(_leftMotor.minPosition, POSITION_ALL_UP), _leftMotor.maxPosition);
//}
//
//position_t PinneRobot::getLeftMotorTargetPosition()
//{
//  return _leftMotor.target;
//}
//
//void PinneRobot::setLeftMotorMaxPosition(position_t maxPos)
//{
//  _leftMotor.maxPosition = maxPos;
//}
//
//position_t PinneRobot::getLeftMotorMaxPosition()
//{
//  return _leftMotor.maxPosition;
//}
//
//void PinneRobot::setLeftMotorMinPosition(position_t minPos)
//{
//  _leftMotor.minPosition = minPos;
//}
//
//position_t PinneRobot::getLeftMotorMinPosition()
//{
//  return _leftMotor.minPosition;
//}
//
//void PinneRobot::setLeftMotorCurrentPosition(position_t pos)
//{
//  posM1 = pos;
//}
//
//position_t PinneRobot::getLeftMotorCurrentPosition()
//{
//  return posM1;
//}
//
//void PinneRobot::setLeftMotorBrake(speed_t brake)
//{
//  _leftMotor.brake = constrain(brake, BRAKE_NONE, BRAKE_FULL);
//  md.setM1Brake(_leftMotor.brake);
//}
//
//speed_t PinneRobot::getLeftMotorBrake()
//{
//  _leftMotor.brake;
//}
//
//void PinneRobot::_leftMotorCalculateAndSetSpeed()
//{
//  if(!_leftMotor.blocked)
//  {
//    md.setM1Speed(_leftMotor.speed * getLeftMotorDirection());
//  }
//}
//
//void PinneRobot::_leftMotorSetBlocked(boolean block)
//{
//  if(_leftMotor.blocked != block)
//  {
//    _leftMotor.blocked = block;
//    _leftMotorCalculateAndSetSpeed();
//  }
//}
//
//void PinneRobot::_leftMotorStopSensorHit()
//{
//  setLeftMotorBrake(BRAKE_FULL);
//  setLeftMotorCurrentPosition(POSITION_ALL_UP);
//  _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_SENSOR);
//}
//
//
//void PinneRobot::_leftMotorTargetReached()
//{
//  setLeftMotorBrake(BRAKE_FULL);
//  _leftMotor.target = TARGET_NONE;
//  _notifyStateChange(LEFT_MOTOR_REACHED_TARGET);
//}
//
//void PinneRobot::_notifyStateChange(stateChange_t stateChange)
//{
//  Serial.print("State change:"); Serial.println(stateChange);
//}
