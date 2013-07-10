#include "PinneAPIParser.h"

PinneAPIParser::PinneAPIParser(PinneRobot* robot) :
  _robot(robot),
  _currentCommand(CMD_UNKNOWN),
  _currentSetGet(SETGET_UNKNOWN),
  _currentAddress(ADDRESS_UNKNOWN)
{
}

void PinneAPIParser::parseIncomingByte(byte inByte)
{
  _parseCommand(inByte);
}

void PinneAPIParser::_parseCommand(byte inByte)
{
  _currentCommand = (command_t)(inByte & PARSE_MASK_COMMAND);
  _currentAddress = (address_t)(inByte & PARSE_MASK_ADDRESS);
  _currentSetGet = (setGet_t)(inByte & PARSE_MASK_SETGET);
  
  switch(_currentSetGet)
  {
    case SET_MESSAGE:
      DEBUG_PRINT("SET: ");
      if(_currentCommand == CMD_STOP)
      {
        DEBUG_PRINT("STOP COMMAND");
        _processSetStopCommand();
      } else {
        if(_getDataBytes())
        {
          switch(_currentCommand)
          {
            case CMD_SPEED:
              DEBUG_PRINT("SET SPEED COMMAND");
              _processSetSpeedCommand();
              break;
            case CMD_DIRECTION:
              DEBUG_PRINT("SET DIRECTION COMMAND");
              _processSetDirectionCommand();
              break;
            case CMD_TARGET_POSITION:
            DEBUG_PRINT("SET TARGET POSITION COMMAND");
              _processSetTargetPositionCommand();
              break;
            case CMD_CURRENT_POSITION:
              DEBUG_PRINT("SET CURRENT POSITION COMMAND");
              _processSetCurrentPositionCommand();
              break;
            case CMD_BRAKE:
              DEBUG_PRINT("SET BRAKE COMMAND");
              _processSetBrakeCommand();
              break;
            default:
              DEBUG_PRINT("Unknown command"); DEBUG_PRINT(_currentCommand);
          }
        } else {
          DEBUG_PRINT("Error getting data bytes");
        }
      }
      break;
    case GET_MESSAGE:
      switch(_currentCommand)
      {
        case CMD_SPEED:
          DEBUG_PRINT("GET SPEED COMMAND");
          _processGetSpeedCommand();
          break;
        case CMD_DIRECTION:
          DEBUG_PRINT("GET DIRECTION COMMAND");
          _processGetDirectionCommand();
          break;
        case CMD_TARGET_POSITION:
          DEBUG_PRINT("GET TARGET POSITION COMMAND");
          _processGetTargetPositionCommand();
          break;
        case CMD_CURRENT_POSITION:
          DEBUG_PRINT("GET TARGET POSITION COMMAND");
          _processGetCurrentPositionCommand();
          break;
        case CMD_BRAKE:
          DEBUG_PRINT("GET BRAKE COMMAND");
          _processGetBrakeCommand();
          break;
        case CMD_STATE:
          DEBUG_PRINT("GET BRAKE COMMAND");
          _processGetStateCommand();
          break;  
        default:
          DEBUG_PRINT("Unknown command"); DEBUG_PRINT(_currentCommand);
      }
      break;
    default:
      DEBUG_PRINT("SetGet fault");
  }
  
  _currentCommand = CMD_UNKNOWN;
  _currentAddress = ADDRESS_UNKNOWN;
  _currentSetGet = SETGET_UNKNOWN;
}

boolean PinneAPIParser::_getDataBytes()
{
  boolean result;
  result = Serial.readBytes(_dataByteBuffer, 2) == 2;
  return result;
}

int PinneAPIParser::_parseDataValue()
{
  int result;
  result = _dataByteBuffer[1] | (_dataByteBuffer[0] << 7);
  return result;
}


void PinneAPIParser::_processSetStopCommand()
{
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Stopping left motor\n");
      _robot->leftMotor->Stop();
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Stopping right motor\n");
      _robot->rightMotor->Stop();
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Stopping rotation motor\n");
      Reply("TEST");
      //_robot->rotationMotor->Stop();
      break;
    case ADDRESS_GLOBAL:
      DEBUG_PRINT("Stopping all motors\n");
      _robot->leftMotor->Stop();
      _robot->rightMotor->Stop();
      //_robot->rotationMotor->Stop();
    default:
      DEBUG_PRINT("Unknown address\n");
  }
}

void PinneAPIParser::_processSetSpeedCommand()
{
  speed_t speed;
  speed = (speed_t)_parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Setting left speed\n"); DEBUG_PRINT(speed);
      _robot->leftMotor->SetSpeed(speed);
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Setting right speed\n"); DEBUG_PRINT(speed);
      _robot->rightMotor->SetSpeed(speed);
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Setting rotation speed\n"); DEBUG_PRINT(speed);
      //_robot->rotationMotor->SetSpeed(speed);
      break;
    default:
      DEBUG_PRINT("Unknown address\n");
  }
}

void PinneAPIParser::_processGetSpeedCommand()
{
  int speed;
  speed = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Left motor speed: ");
      speed = _robot->leftMotor->GetSpeed();
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Right motor speed: ");
      speed = _robot->rightMotor->GetSpeed();
      break;
//    case ADDRESS_ROTATION:
//      DEBUG_PRINT("Right motor speed: ");
//      speed = _robot->rotationMotor->GetSpeed();
//      break;
    default:
      DEBUG_PRINT("Unknown address\n");
  }
  if(speed >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, speed);
  } else {
    DEBUG_PRINT("Something wrong with geting speed");
  }
}

void PinneAPIParser::_processSetDirectionCommand()
{
  int direction = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Setting left direction"); DEBUG_PRINT(direction);
      _robot->leftMotor->SetDirection(direction);
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Setting right direction"); DEBUG_PRINT(direction);
      _robot->rightMotor->SetDirection(direction);
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Setting rotation direction"); DEBUG_PRINT(direction);
      //_robot->rotationMotor->SetDirection(direction);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetDirectionCommand()
{
  int direction;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //direction = _robot->getLeftMotorDirection();
      DEBUG_PRINT("Left motor direction: "); DEBUG_PRINT(direction);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processSetTargetPositionCommand()
{
  int targetPosition;
  targetPosition = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Setting left targetPosition"); DEBUG_PRINT(targetPosition);
      //_robot->setLeftMotorTargetPosition(targetPosition);
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Setting right targetPosition"); DEBUG_PRINT(targetPosition);
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Setting rotation targetPosition"); DEBUG_PRINT(targetPosition);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetTargetPositionCommand()
{
  int targetPosition;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //targetPosition = _robot->getLeftMotorTargetPosition();
      DEBUG_PRINT("Left motor targetPosition: "); DEBUG_PRINT(targetPosition);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processSetCurrentPositionCommand()
{
  int currentPosition;
  currentPosition = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Setting left currentPosition"); DEBUG_PRINT(currentPosition);
      //_robot->setLeftMotorCurrentPosition(currentPosition);
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Setting right currentPosition"); DEBUG_PRINT(currentPosition);
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Setting rotation currentPosition"); DEBUG_PRINT(currentPosition);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetCurrentPositionCommand()
{
  int currentPosition;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //currentPosition = _robot->getLeftMotorCurrentPosition();
      DEBUG_PRINT("Left motor currentPosition: "); DEBUG_PRINT(currentPosition);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processSetBrakeCommand()
{
  int brake;
  brake = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      DEBUG_PRINT("Setting left brake"); DEBUG_PRINT(brake);
      //_robot->setLeftMotorBrake(brake);
      break;
    case ADDRESS_RIGHT:
      DEBUG_PRINT("Setting right brake"); DEBUG_PRINT(brake);
      break;
    case ADDRESS_ROTATION:
      DEBUG_PRINT("Setting rotation brake"); DEBUG_PRINT(brake);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetBrakeCommand()
{
  int brake;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //brake = _robot->getLeftMotorBrake();
      DEBUG_PRINT("Left motor brake: "); DEBUG_PRINT(brake);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetStateCommand()
{
  DEBUG_PRINT("State command");
}


