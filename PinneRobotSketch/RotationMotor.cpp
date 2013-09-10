#include "Motor.h"

const int RotationMotor::DIRECTION_RIGHT = 0;
const int RotationMotor::DIRECTION_LEFT = 1;
const int RotationMotor::POSITION_MAX_RIGHT = 200;
const int RotationMotor::POSITION_MAX_LEFT = 0;
const int RotationMotor::SPEED_STOP = 0;

RotationMotor::RotationMotor(unsigned char rotationPotmeterPin, L293Driver* driver, address_t address) :
  _driver(driver),
  _rotationPotmeterPin(rotationPotmeterPin),
  _address(address)
{}

void RotationMotor::init()
{
  _driver->init();
  _driver->SetSpeed(0);
  _driver->SetDirection(DIRECTION_LEFT);
  pinMode(_rotationPotmeterPin, INPUT);
}

void RotationMotor::SetSpeed(int speed)
{
  _driver->SetSpeed(speed);
}

void RotationMotor::SetDirection(int direction)
{
  _driver->SetDirection(direction);
}

void RotationMotor::SetBrake(int brake)
{
  _driver->SetBrake(brake);
}

void RotationMotor::SetTargetPosition(int targetPosition)
{
  _targetPosition = constrain(targetPosition, POSITION_MAX_RIGHT, POSITION_MAX_LEFT);
}

void RotationMotor::SetMinPosition(int minPosition)
{
  _rightMaxPosition = max(minPosition, POSITION_MAX_RIGHT);
}

void RotationMotor::SetMaxPosition(int minPosition)
{
  _leftMaxPosition = max(minPosition, POSITION_MAX_LEFT);
}

//Read potmeter and stop if at target or outside boundary positions
void RotationMotor::UpdateState()
{

}

boolean RotationMotor::IsBlocked()
{
  return false;
}
