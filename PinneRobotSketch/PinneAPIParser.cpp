#include "PinneRobot.h"
PinneRobot robot;

void processSetTargetPositionCommand()
{
  position_t targetPosition;
  targetPosition = (position_t)parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left targetPosition"); Serial.println(targetPosition);
      parser.robot->setLeftMotorTargetPosition(targetPosition);
      break;
    case ADDRESS_RIGHT:
      Serial.print("Setting right targetPosition"); Serial.println(targetPosition);
      break;
    case ADDRESS_ROTATION:
      Serial.print("Setting rotation targetPosition"); Serial.println(targetPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetTargetPositionCommand()
{
  Serial.print("Target position command: ");
}

void processSetCurrentPositionCommand()
{
  position_t currentPosition;
  currentPosition = (position_t)parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left currentPosition"); Serial.println(currentPosition);
      parser.robot->setLeftMotorCurrentPosition(currentPosition);
      break;
    case ADDRESS_RIGHT:
      Serial.print("Setting right currentPosition"); Serial.println(currentPosition);
      break;
    case ADDRESS_ROTATION:
      Serial.print("Setting rotation currentPosition"); Serial.println(currentPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetCurrentPositionCommand()
{
  Serial.print("Current position command: ");
}

void processSetBrakeCommand()
{
  speed_t brake;
  brake = (speed_t)parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left brake"); Serial.println(brake);
      parser.robot->setLeftMotorBrake(brake);
      break;
    case ADDRESS_RIGHT:
      Serial.print("Setting right brake"); Serial.println(brake);
      break;
    case ADDRESS_ROTATION:
      Serial.print("Setting rotation brake"); Serial.println(brake);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetBrakeCommand()
{
  Serial.print("Brake command: ");
}
