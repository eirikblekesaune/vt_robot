#include "PinneRobot.h"

//pin connections
//left driver aka motor 1, right driver motor 2
const int leftDriverPWM = 9;
const int leftDriverINA = 11;//Remapped due to interrupt
const int leftDriverINB = 4;
const int leftMotorSlackStopButton = A0; //todo: cut trace, was current sense pin
const int leftDriverENDIAG = 6;
const int leftMotorEncoderInterruptIndex = 1;// digital pin 3 on Leonardo implicitly
const int leftMotorTopStopButton = A5;

const int rightDriverPWM = 10;
const int rightDriverINA = 7;
const int rightDriverINB = 8;
const int rightMotorSlackStopButton = A1;//todo cut trace, was current sense pin
const int rightDriverENDIAG = 12;
const int rightMotorEncoderInterruptIndex = 0;// digital pin 2 on Leonardo implicitly
const int rightMotorTopStopButton = 13;

const int rotationDriver1A = A3;
const int rotationDriver2A = A4;
const int rotationDriverPWM = 5;

PinneRobot::PinneRobot()
{
  VNH5019Driver *leftDriver = new VNH5019Driver(leftDriverINA, leftDriverINB, leftDriverENDIAG, leftDriverPWM);
  VNH5019Driver *rightDriver = new VNH5019Driver(rightDriverINA, rightDriverINB, rightDriverENDIAG, rightDriverPWM);
  leftMotor = new PinneMotor(leftMotorTopStopButton, leftMotorSlackStopButton, leftMotorEncoderInterruptIndex, leftDriver, ADDRESS_LEFT);
  rightMotor = new PinneMotor(rightMotorTopStopButton, rightMotorSlackStopButton, rightMotorEncoderInterruptIndex, rightDriver, ADDRESS_RIGHT);
}

void PinneRobot::init()
{
  leftMotor->init();
  rightMotor->init();
}

void PinneRobot::update()
{
  leftMotor->UpdateState();
  rightMotor->UpdateState();
}


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
//  DEBUG_PRINT("State change:"); DEBUG_PRINT(stateChange);
//}
