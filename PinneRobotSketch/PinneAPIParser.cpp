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
      if(_currentCommand == CMD_STOP)
      {
        _processSetStopCommand();
      } else {
        if(_getDataBytes())
        {
          switch(_currentCommand)
          {
            case CMD_SPEED:
              _processSetSpeedCommand();
              break;
            case CMD_DIRECTION:
              _processSetDirectionCommand();
              break;
            case CMD_TARGET_POSITION:
              _processSetTargetPositionCommand();
              break;
            case CMD_CURRENT_POSITION:
              _processSetCurrentPositionCommand();
              break;
            case CMD_BRAKE:
              _processSetBrakeCommand();
              break;
            case CMD_MIN_POSITION:
              _processSetMinPositionCommand();
              break;
            case CMD_MAX_POSITION:
              _processSetMaxPositionCommand();
              break;
            case CMD_WRITE_SETTINGS:
              _processWriteDataCommand();
              break;
            case CMD_LOAD_SETTINGS:
              _processLoadSettingsCommand();
              break;
            default:
              DebugPrint("Unknown command");
              DebugPrint(_currentCommand);
          }
        } else {
          ////DEBUG_PRINT("Error getting data bytes");DEBUG_NL;
        }
      }
      break;
    case GET_MESSAGE:
      switch(_currentCommand)
      {
        case CMD_SPEED:
          ////DEBUG_PRINT("GET SPEED COMMAND");DEBUG_NL;
          _processGetSpeedCommand();
          break;
        case CMD_DIRECTION:
          ////DEBUG_PRINT("GET DIRECTION COMMAND");DEBUG_NL;
          _processGetDirectionCommand();
          break;
        case CMD_TARGET_POSITION:
          ////DEBUG_PRINT("GET TARGET POSITION COMMAND");DEBUG_NL;
          _processGetTargetPositionCommand();
          break;
        case CMD_CURRENT_POSITION:
          ////DEBUG_PRINT("GET TARGET POSITION COMMAND");DEBUG_NL;
          _processGetCurrentPositionCommand();
          break;
        case CMD_BRAKE:
          ////DEBUG_PRINT("GET BRAKE COMMAND");DEBUG_NL;
          _processGetBrakeCommand();
          break;
        case CMD_STATE_CHANGE:
          ////DEBUG_PRINT("GET STATE CHANGE COMMAND");DEBUG_NL;
          _processGetStateCommand();
          break;
        case CMD_MIN_POSITION:
          ////DEBUG_PRINT("GET MIN POSITION COMMAND");DEBUG_NL;
          _processGetMinPositionCommand();
          break;
        case  CMD_MAX_POSITION:
          ////DEBUG_PRINT("GET MAX POSITION COMMAND");DEBUG_NL;
          _processGetMaxPositionCommand();
          break;  
        default:
          DEBUG_PRINT("Unknown command"); //DEBUG_PRINT(_currentCommand);DEBUG_NL;
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
  result = Serial1.readBytes(_dataByteBuffer, 2) == 2;
  return result;
}

int PinneAPIParser::_parseDataValue()
{
  int result;
  result = _dataByteBuffer[1] | (_dataByteBuffer[0] << 7);
  return result;
}



//STOP COMMAND
void PinneAPIParser::_processSetStopCommand()
{
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      ////DEBUG_PRINT("Stopping left motor\n");DEBUG_NL;
      _robot->leftMotor->Stop();
      break;
    case ADDRESS_RIGHT:
      ////DEBUG_PRINT("Stopping right motor\n");DEBUG_NL;
      _robot->rightMotor->Stop();
      break;
    case ADDRESS_ROTATION:
      ////DEBUG_PRINT("Stopping rotation motor\n");DEBUG_NL;
      _robot->rotationMotor->Stop();
      break;
    case ADDRESS_GLOBAL:
      ////DEBUG_PRINT("Stopping all motors\n");DEBUG_NL;
      _robot->leftMotor->Stop();
      _robot->rightMotor->Stop();
      _robot->rotationMotor->Stop();
    default:
      DEBUG_PRINT("Unknown address\n");
  }
}


//SPPED COMMAND
void PinneAPIParser::_processSetSpeedCommand()
{
  int value;
  value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      _robot->leftMotor->SetSpeed(value);
      break;
    case ADDRESS_RIGHT:
      _robot->rightMotor->SetSpeed(value);
      break;
    case ADDRESS_ROTATION:
      _robot->rotationMotor->SetSpeed(value);
      break;
    default:
      DEBUG_PRINT("Unknown address\n");
  }
}

void PinneAPIParser::_processGetSpeedCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      value = _robot->leftMotor->GetSpeed();
      break;
    case ADDRESS_RIGHT:
      value = _robot->rightMotor->GetSpeed();
      break;
    case ADDRESS_ROTATION:
      value = _robot->rotationMotor->GetSpeed();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    ////DEBUG_PRINT("Something wrong with geting speed");DEBUG_NL;
  }
}


//DIRECTION COMMAND
void PinneAPIParser::_processSetDirectionCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      ////DEBUG_PRINT("Setting left direction"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetDirection(value);
      break;
    case ADDRESS_RIGHT:
      ////DEBUG_PRINT("Setting right direction"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetDirection(value);
      break;
    case ADDRESS_ROTATION:
      ////DEBUG_PRINT("Setting rotation direction"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rotationMotor->SetDirection(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");DEBUG_NL;
  }
}

void PinneAPIParser::_processGetDirectionCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      ////DEBUG_PRINT("Left motor speed: ");DEBUG_NL;
      value = _robot->leftMotor->GetDirection();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor speed: ");DEBUG_NL;
      value = _robot->rightMotor->GetDirection();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor speed: ");
      value = _robot->rotationMotor->GetDirection();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting direction");DEBUG_NL;
  }
}


////TARGET POSITION COMMAND
void PinneAPIParser::_processSetTargetPositionCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Setting left target postition"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetTargetPosition(value);
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Setting right target postition"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetTargetPosition(value);
      break;
    case ADDRESS_ROTATION:
      //DEBUG_PRINT("Setting rotation target postition"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rotationMotor->SetTargetPosition(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");DEBUG_NL;
  }
}

void PinneAPIParser::_processGetTargetPositionCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Left motor target position: ");DEBUG_NL;
      value = _robot->leftMotor->GetTargetPosition();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor target position: ");DEBUG_NL;
      value = _robot->rightMotor->GetTargetPosition();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor target position: ");
      value = _robot->rotationMotor->GetTargetPosition();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting target position");DEBUG_NL;
  }
}

////CURRENT POSITION COMMAND
void PinneAPIParser::_processSetCurrentPositionCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Setting left current postition"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetCurrentPosition(value);
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Setting right current postition"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetCurrentPosition(value);
      break;
    case ADDRESS_ROTATION:
      //DEBUG_PRINT("Setting rotation current postition"); //DEBUG_PRINT(value);DEBUG_NL;
      //We don't allow setting rotation posistion since it is absolute.
      //_robot->rotationMotor->SetCurrentPosition(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");DEBUG_NL;
  }
}

void PinneAPIParser::_processGetCurrentPositionCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Left motor current position: ");DEBUG_NL;
      value = _robot->leftMotor->GetCurrentPosition();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor current position: ");DEBUG_NL;
      value = _robot->rightMotor->GetCurrentPosition();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor current position: ");
      value = _robot->rotationMotor->GetCurrentPosition();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting current brake");DEBUG_NL;
  }
}


////BRAKE COMMAND
void PinneAPIParser::_processSetBrakeCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Setting left brake"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetBrake(value);
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Setting right brake"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetBrake(value);
      break;
    case ADDRESS_ROTATION:
      //DEBUG_PRINT("Setting rotation brake"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rotationMotor->SetBrake(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");DEBUG_NL;
  }
}

void PinneAPIParser::_processGetBrakeCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Left motor brake: ");DEBUG_NL;
      value = _robot->leftMotor->GetBrake();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor brake: ");DEBUG_NL;
      value = _robot->rightMotor->GetBrake();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor brake: ");DEBUG_NL;
      value = _robot->rotationMotor->GetBrake();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting current brake");DEBUG_NL;
  }
}


////MIN POSITION COMMAND
void PinneAPIParser::_processSetMinPositionCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Setting left min position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetMinPosition(value);
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Setting right min position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetMinPosition(value);
      break;
    case ADDRESS_ROTATION:
      //DEBUG_PRINT("Setting rotation min position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rotationMotor->SetMinPosition(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");DEBUG_NL;
  }
}

void PinneAPIParser::_processGetMinPositionCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Left motor min position: ");DEBUG_NL;
      value = _robot->leftMotor->GetMinPosition();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor min position: ");DEBUG_NL;
      value = _robot->rightMotor->GetMinPosition();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor min position: ");DEBUG_NL;
      value = _robot->rotationMotor->GetMinPosition();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting min position");DEBUG_NL;
  }
}

////MAX POSITION COMMAND
void PinneAPIParser::_processSetMaxPositionCommand()
{
  int value = _parseDataValue();
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Setting left max position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->leftMotor->SetMaxPosition(value);
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Setting right max position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rightMotor->SetMaxPosition(value);
      break;
    case ADDRESS_ROTATION:
      //DEBUG_PRINT("Setting rotation max position"); //DEBUG_PRINT(value);DEBUG_NL;
      _robot->rotationMotor->SetMaxPosition(value);
      break;
    default:
      DEBUG_PRINT("Unknown address");
  }
}

void PinneAPIParser::_processGetMaxPositionCommand()
{
  int value;
  value = -1;
  switch(_currentAddress)
  {
    case ADDRESS_LEFT:
      //DEBUG_PRINT("Left motor max position: ");DEBUG_NL;
      value = _robot->leftMotor->GetMaxPosition();
      break;
    case ADDRESS_RIGHT:
      //DEBUG_PRINT("Right motor max position: ");DEBUG_NL;
      value = _robot->rightMotor->GetMaxPosition();
      break;
    case ADDRESS_ROTATION:
//      //DEBUG_PRINT("Right motor max position: ");DEBUG_NL;
      value = _robot->rotationMotor->GetMaxPosition();
      break;
    default:
      DEBUG_PRINT("Unknown address\n");DEBUG_NL;
  }
  if(value >= 0)
  {
    ReturnGetValue(_currentCommand, _currentAddress, value);
  } else {
    //DEBUG_PRINT("Something wrong with geting max position");DEBUG_NL;
  }
}


void PinneAPIParser::_processGetStateCommand()
{
  //DEBUG_PRINT("State command");DEBUG_NL;
}

void PinneAPIParser::_processWriteDataCommand()
{
  int value = _parseDataValue();
  if(value != 9999)
  {
    DebugPrint("Value needs to be 9999 in order to store");
  } else {
    _robot->storeSettingsToEEPROM();
  }
}

void PinneAPIParser::_processLoadSettingsCommand()
{
  int value = _parseDataValue();
  if(value != 9999)
  {
    DebugPrint("Value needs to be 9999 in order to load");
  } else {
    _robot->loadSettingsFromEEPROM();
  }
}
