#include "PinneRobot.h"

DualVNH5019MotorShield md(
  M1_INA, M1_INB, M1_CS,
  M2_INA, M2_INB, M2_CS
);


volatile position_t posM1 = 0;
volatile position_t posM2 = 0;
volatile direction_t directionM1 = DIRECTION_DOWN;
volatile direction_t directionM2 = DIRECTION_DOWN;

//Encoder reading ISR code based on:
//http://makeatronics.blogspot.no/2013/02/efficiently-reading-quadrature-with.html
void encoder1_ISR()
{
  posM1 = posM1 + directionM1;
}

void encoder2_ISR()
{
  posM2 = posM2 + directionM2;
}

void PinneRobot::init()
{
  md.init();
  attachInterrupt(0, encoder1_ISR, CHANGE);
  attachInterrupt(1, encoder2_ISR, CHANGE);
  pinMode(M1_HALLA, INPUT);
  pinMode(M1_HALLB, INPUT);
  pinMode(M1_STOP, INPUT);
  pinMode(M2_HALLA, INPUT);
  pinMode(M2_HALLB, INPUT);
  pinMode(M2_STOP, INPUT);
  pinMode(L293_1A, OUTPUT);
  pinMode(L293_2A, OUTPUT);
  pinMode(L293_EN, OUTPUT);
  
  _initLeftMotor();
}

void PinneRobot::_initLeftMotor()
{
  _leftMotor.speed = SPEED_STOP;
  directionM1 = DIRECTION_DOWN;
  _leftMotor.target = TARGET_NONE;
  _leftMotor.minPosition = POSITION_ALL_UP;
  _leftMotor.maxPosition = POSITION_DEFAULT_MAX;
  _leftMotor.brake = BRAKE_NONE;
  _leftMotor.stopButtonValue = digitalRead(M1_STOP);
}

void PinneRobot::checkSensors()
{
  int i, val;
  val = digitalRead(M1_STOP);
  if( val != _leftMotor.stopButtonValue)
  {
    if(val == BUTTON_IN)
    {
      _leftMotorStopSensorHit();
    } else {
      setLeftMotorBrake(BRAKE_NONE);
    }
  }
}

void PinneRobot::updatePositions()
{
  position_t currentPosition;
  if(_leftMotor.target != TARGET_NONE)
  {
    if(getLeftMotorDirection() == DIRECTION_UP)
    {
      if(posM1 <= _leftMotor.target)
      {
        _leftMotorTargetReached();
      }
    } else {//otherwise the direction is DIRECTION_DOWN
      if(posM1 >= _leftMotor.target)
      {
        _leftMotorTargetReached();
      }
    }
  }
  currentPosition = getLeftMotorCurrentPosition();
  if(!_leftMotor.blocked)
    {
    if(currentPosition <= POSITION_ALL_UP)
    {
      _leftMotorSetBlocked(true);
      _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_MIN_POSITION);
    }
    if(currentPosition >= getLeftMotorMaxPosition())
    {
      _leftMotorSetBlocked(true);
      _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_MAX_POSITION);
    }
  }
}

void PinneRobot::setLeftMotorSpeed(speed_t speed)
{
  _leftMotor.speed = constrain(speed, SPEED_MIN, SPEED_MAX);
  if(_leftMotor.speed == 0)
  {
    leftMotorStop();
  } else {
    _leftMotorCalculateAndSetSpeed();
  }
}

speed_t PinneRobot::getLeftMotorSpeed()
{
  return _leftMotor.speed;
}

void PinneRobot::leftMotorStop()
{
  _leftMotor.speed = SPEED_STOP;
  _leftMotorCalculateAndSetSpeed();
  _notifyStateChange(LEFT_MOTOR_STOPPED);
}

void PinneRobot::setLeftMotorDirection(direction_t direction)
{
  directionM1 = direction;
  _leftMotorCalculateAndSetSpeed();
  if(getLeftMotorDirection() == DIRECTION_UP)
  {
    _notifyStateChange(LEFT_MOTOR_GOING_UP);
  } else {
    _notifyStateChange(LEFT_MOTOR_GOING_DOWN);
  }
}

direction_t PinneRobot::getLeftMotorDirection()
{
  return directionM1;
}

void PinneRobot::setLeftMotorTargetPosition(position_t position)
{
  _leftMotor.target = constrain(position, max(_leftMotor.minPosition, POSITION_ALL_UP), _leftMotor.maxPosition);
}

position_t PinneRobot::getLeftMotorTargetPosition()
{
  return _leftMotor.target;
}

void PinneRobot::setLeftMotorMaxPosition(position_t maxPos)
{
  _leftMotor.maxPosition = maxPos;
}

position_t PinneRobot::getLeftMotorMaxPosition()
{
  return _leftMotor.maxPosition;
}

void PinneRobot::setLeftMotorMinPosition(position_t minPos)
{
  _leftMotor.minPosition = minPos;
}

position_t PinneRobot::getLeftMotorMinPosition()
{
  return _leftMotor.minPosition;
}

void PinneRobot::setLeftMotorCurrentPosition(position_t pos)
{
  posM1 = pos;
}

position_t PinneRobot::getLeftMotorCurrentPosition()
{
  return posM1;
}

void PinneRobot::setLeftMotorBrake(speed_t brake)
{
  _leftMotor.brake = constrain(brake, BRAKE_NONE, BRAKE_FULL);
  md.setM1Brake(_leftMotor.brake);
}

speed_t PinneRobot::getLeftMotorBrake()
{
  _leftMotor.brake;
}

void PinneRobot::_leftMotorCalculateAndSetSpeed()
{
  if(!_leftMotor.blocked)
  {
    md.setM1Speed(_leftMotor.speed * getLeftMotorDirection());
  }
}

void PinneRobot::_leftMotorSetBlocked(boolean block)
{
  if(_leftMotor.blocked != block)
  {
    _leftMotor.blocked = block;
    _leftMotorCalculateAndSetSpeed();
  }
}

void PinneRobot::_leftMotorStopSensorHit()
{
  setLeftMotorBrake(BRAKE_FULL);
  setLeftMotorCurrentPosition(POSITION_ALL_UP);
  _notifyStateChange(LEFT_MOTOR_BLOCKED_BY_SENSOR);
}


void PinneRobot::_leftMotorTargetReached()
{
  setLeftMotorBrake(BRAKE_FULL);
  _leftMotor.target = TARGET_NONE;
  _notifyStateChange(LEFT_MOTOR_REACHED_TARGET);
}

void PinneRobot::_notifyStateChange(stateChange_t stateChange)
{
  Serial.print("State change:"); Serial.println(stateChange);
}
