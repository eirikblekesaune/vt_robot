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
      Serial.print("SET: ");
      if(_currentCommand == CMD_STOP)
      {
        Serial.println("SET STOP COMMAND");
        _processSetStopCommand();
      } else {
        if(_getDataBytes())
        {
          switch(_currentCommand)
          {
            case CMD_SPEED:
              Serial.println("SET SPEED COMMAND");
              _processSetSpeedCommand();
              break;
            case CMD_DIRECTION:
              Serial.println("SET DIRECTION COMMAND");
              _processSetDirectionCommand();
              break;
            case CMD_TARGET_POSITION:
            Serial.println("SET TARGET POSITION COMMAND");
              _processSetTargetPositionCommand();
              break;
            case CMD_CURRENT_POSITION:
              Serial.println("SET CURRENT POSITION COMMAND");
              _processSetCurrentPositionCommand();
              break;
            case CMD_BRAKE:
              Serial.println("SET BRAKE COMMAND");
              _processSetBrakeCommand();
              break;
            default:
              Serial.print("Unknown command"); Serial.println(_currentCommand);
          }
        } else {
          Serial.println("Error getting data bytes");
        }
      }
      break;
    case GET_MESSAGE:
      switch(_currentCommand)
      {
        case CMD_SPEED:
          Serial.println("GET SPEED COMMAND");
          _processGetSpeedCommand();
          break;
        case CMD_DIRECTION:
          Serial.println("GET DIRECTION COMMAND");
          _processGetDirectionCommand();
          break;
        case CMD_TARGET_POSITION:
          Serial.println("GET TARGET POSITION COMMAND");
          _processGetTargetPositionCommand();
          break;
        case CMD_CURRENT_POSITION:
          Serial.println("GET TARGET POSITION COMMAND");
          _processGetCurrentPositionCommand();
          break;
        case CMD_BRAKE:
          Serial.println("GET BRAKE COMMAND");
          _processGetBrakeCommand();
          break;
        case CMD_STATE:
          Serial.println("GET BRAKE COMMAND");
          _processGetStateCommand();
          break;  
        default:
          Serial.print("Unknown command"); Serial.println(_currentCommand);
      }
      break;
    default:
      Serial.println("SetGet fault");
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
      Serial.print("Stopping left motor");
      _robot->leftMotor->Stop();
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

void PinneAPIParser::_processSetSpeedCommand()
{
  speed_t speed;
  speed = (speed_t)_parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left speed"); Serial.println(speed);
      _robot->leftMotor->SetSpeed(speed);
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

void PinneAPIParser::_processGetSpeedCommand()
{
  int speed;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Left motor speed: ");
      //speed = _robot->getLeftMotorSpeed();
      Serial.println(speed);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void PinneAPIParser::_processSetDirectionCommand()
{
  int direction;
  direction = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left direction"); Serial.println(direction);
      //_robot->setLeftMotorDirection(direction);
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

void PinneAPIParser::_processGetDirectionCommand()
{
  int direction;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //direction = _robot->getLeftMotorDirection();
      Serial.print("Left motor direction: "); Serial.println(direction);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void PinneAPIParser::_processSetTargetPositionCommand()
{
  int targetPosition;
  targetPosition = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left targetPosition"); Serial.println(targetPosition);
      //_robot->setLeftMotorTargetPosition(targetPosition);
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

void PinneAPIParser::_processGetTargetPositionCommand()
{
  int targetPosition;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //targetPosition = _robot->getLeftMotorTargetPosition();
      Serial.print("Left motor targetPosition: "); Serial.println(targetPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void PinneAPIParser::_processSetCurrentPositionCommand()
{
  int currentPosition;
  currentPosition = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left currentPosition"); Serial.println(currentPosition);
      //_robot->setLeftMotorCurrentPosition(currentPosition);
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

void PinneAPIParser::_processGetCurrentPositionCommand()
{
  int currentPosition;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //currentPosition = _robot->getLeftMotorCurrentPosition();
      Serial.print("Left motor currentPosition: "); Serial.println(currentPosition);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void PinneAPIParser::_processSetBrakeCommand()
{
  int brake;
  brake = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      Serial.print("Setting left brake"); Serial.println(brake);
      //_robot->setLeftMotorBrake(brake);
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

void PinneAPIParser::_processGetBrakeCommand()
{
  int brake;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //brake = _robot->getLeftMotorBrake();
      Serial.print("Left motor brake: "); Serial.println(brake);
      break;
    default:
      Serial.println("Unknown address");
  }
}

void PinneAPIParser::_processGetStateCommand()
{
  Serial.println("State command");
}
