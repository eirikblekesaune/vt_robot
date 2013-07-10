#include "PinneAPIParser.h"

extern PinneParser parser;

void parseIncomingByte(byte inByte)
{
  if(parser.state == WAITING_FOR_COMMAND_BYTE)
  {
    parseCommand(inByte);
  }
}

void parseCommand(byte inByte)
{
  parser.currentCommand = (command_t)(inByte & PARSE_MASK_COMMAND);
  parser.currentAddress = (address_t)(inByte & PARSE_MASK_ADDRESS);
  parser.currentSetGet = (setGet_t)(inByte & PARSE_MASK_SETGET);
  switch(parser.currentSetGet)
  {
    case SET_MESSAGE:
      Serial.print("SET: ");
      if(parser.currentCommand == CMD_STOP)
      {
        processSetStopCommand();
      } else {
        if(getDataBytes())
        {
          switch(parser.currentCommand)
          {
            case CMD_SPEED:
              processSetSpeedCommand();
              break;
            case CMD_DIRECTION:
              processSetDirectionCommand();
              break;
            case CMD_TARGET_POSITION:
              processSetTargetPositionCommand();
              break;
            case CMD_CURRENT_POSITION:
              processSetCurrentPositionCommand();
              break;
            case CMD_BRAKE:
              processSetBrakeCommand();
              break;
            default:
              Serial.print("Unknown command"); Serial.println(parser.currentCommand);
          }
        } else {
          Serial.println("Error getting data bytes");
        }
      }
      break;
    case GET_MESSAGE:
      switch(parser.currentCommand)
      {
        case CMD_SPEED:
          processGetSpeedCommand();
          break;
        case CMD_DIRECTION:
          processGetDirectionCommand();
          break;
        case CMD_TARGET_POSITION:
          processGetTargetPositionCommand();
          break;
        case CMD_CURRENT_POSITION:
          processGetCurrentPositionCommand();
          break;
        case CMD_BRAKE:
          processGetBrakeCommand();
          break;
        case CMD_STATE:
          processGetStateCommand();
          break;  
        default:
          Serial.print("Unknown command"); Serial.println(parser.currentCommand);
      }
      break;
    default:
      Serial.println("SetGet fault");
  }
}

boolean getDataBytes()
{
  boolean result;
  result = Serial.readBytes(parser.dataByteBuffer, 2) == 2;
  return result;
}

int parseDataValue()
{
  int result;
  result = parser.dataByteBuffer[1] | (parser.dataByteBuffer[0] << 7);
  return result;
}

void processSetStopCommand()
{
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Stopping left motor");
      break;
    case ADDRESS_RIGHT:
      Serial.print("Stopping right motor");
      break;
    case ADDRESS_ROTATION:
      Serial.print("Stopping rotation motor");
      break;
    case ADDRESS_GLOBAL:
      Serial.print("Stopping all motors");
    default:
      Serial.println("Unknown address");
  }
}

void processSetSpeedCommand()
{
  int speed;
  speed = parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left speed"); Serial.println(speed);
      //parser.robot->setLeftMotorSpeed(speed);
      break;
    case ADDRESS_RIGHT:
      Serial.print("Setting right speed"); Serial.println(speed);
      break;
    case ADDRESS_ROTATION:
      Serial.print("Setting rotation speed"); Serial.println(speed);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetSpeedCommand()
{
  int speed;
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Left motor speed: ");
      //speed = parser.robot->getLeftMotorSpeed();
      Serial.println(speed);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processSetDirectionCommand()
{
  int direction;
  direction = parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left direction"); Serial.println(direction);
      //parser.robot->setLeftMotorDirection(direction);
      break;
    case ADDRESS_RIGHT:
      Serial.print("Setting right direction"); Serial.println(direction);
      break;
    case ADDRESS_ROTATION:
      Serial.print("Setting rotation direction"); Serial.println(direction);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetDirectionCommand()
{
  int direction;
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      //direction = parser.robot->getLeftMotorDirection();
      Serial.print("Left motor direction: "); Serial.println(direction);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processSetTargetPositionCommand()
{
  int targetPosition;
  targetPosition = parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left targetPosition"); Serial.println(targetPosition);
      //parser.robot->setLeftMotorTargetPosition(targetPosition);
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
  int targetPosition;
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      //targetPosition = parser.robot->getLeftMotorTargetPosition();
      Serial.print("Left motor targetPosition: "); Serial.println(targetPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processSetCurrentPositionCommand()
{
  int currentPosition;
  currentPosition = parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left currentPosition"); Serial.println(currentPosition);
      //parser.robot->setLeftMotorCurrentPosition(currentPosition);
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
  int currentPosition;
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      //currentPosition = parser.robot->getLeftMotorCurrentPosition();
      Serial.print("Left motor currentPosition: "); Serial.println(currentPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processSetBrakeCommand()
{
  int brake;
  brake = parseDataValue();
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left brake"); Serial.println(brake);
      //parser.robot->setLeftMotorBrake(brake);
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
  int brake;
  switch(parser.currentAddress)
  {
    case ADDRESS_LEFT:
      //brake = parser.robot->getLeftMotorBrake();
      Serial.print("Left motor brake: "); Serial.println(brake);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void processGetStateCommand()
{
  Serial.println("State command");
}
