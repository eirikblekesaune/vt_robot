#include "KoppAPIParser.h"

KoppAPIParser::KoppAPIParser(KoppRobot* robot) :
  _robot(robot),
  _currentCommand(CMD_UNKNOWN),
  _currentSetGet(SETGET_UNKNOWN)
{
}

void KoppAPIParser::parseIncomingByte(byte inByte)
{
  _parseCommand(inByte);
}

void KoppAPIParser::_parseCommand(byte inByte)
{
	int value;
  _currentCommand = (command_t)(inByte & PARSE_MASK_COMMAND);
  _currentSetGet = (setGet_t)(inByte & PARSE_MASK_SETGET);	
  switch(_currentSetGet)
  {
    case SET_MESSAGE:
      _getDataBytes();
			value = _parseDataValue();
          switch(_currentCommand)
          {
            case CMD_STOP:
							_robot->motor->SetStop(value);
              break;
            case CMD_SPEED:
							_robot->motor->SetSpeed(value);
              break;
            case CMD_DIRECTION:
							_robot->motor->SetDirection(value);
							break;
            case CMD_TARGET_POSITION:
							_robot->motor->SetTargetPosition(value);
              break;
            case CMD_CURRENT_POSITION:
							_robot->motor->SetCurrentPosition(value);
              break;
            case CMD_BRAKE:
							_robot->motor->SetBrake(value);
              break;
            case CMD_MIN_POSITION:
							_robot->motor->SetMinPosition(value);
              break;
            case CMD_MAX_POSITION:
							_robot->motor->SetMaxPosition(value);
              break;
            case CMD_GOTO_PARKING_POSITION:
              _robot->GoToParkingPosition();
              break;
            case CMD_GOTO_TARGET:
							_robot->motor->GoToTargetPosition(value);
              break;
            case CMD_GOTO_SPEED_RAMP_UP:
							_robot->motor->SetGoToSpeedRampUp(value);
              break;
            case CMD_GOTO_SPEED_RAMP_DOWN:
							_robot->motor->SetGoToSpeedRampDown(value);
              break;
            case CMD_GOTO_SPEED_SCALING:
							_robot->motor->SetGoToSpeedScaling(value);
              break;
            default:
              DebugPrint("Unknown command");
              DebugPrint(_currentCommand);
          }
          break;
    case GET_MESSAGE:
					value = -1;
          switch(_currentCommand)
          {
            case CMD_STOP:
							value = _robot->motor->GetStop();
              break;
            case CMD_SPEED:
							value = _robot->motor->GetSpeed();
              break;
            case CMD_DIRECTION:
							value = _robot->motor->GetDirection();
							break;
            case CMD_TARGET_POSITION:
							value = _robot->motor->GetTargetPosition();
              break;
            case CMD_CURRENT_POSITION:
							value = _robot->motor->GetCurrentPosition();
              break;
            case CMD_BRAKE:
							value = _robot->motor->GetBrake();
              break;
            case CMD_MIN_POSITION:
							value = _robot->motor->GetMinPosition();
              break;
            case CMD_MAX_POSITION:
							value = _robot->motor->GetMaxPosition();
              break;
            case CMD_GOTO_PARKING_POSITION:
            case CMD_GOTO_TARGET:
              break;
            case CMD_GOTO_SPEED_RAMP_UP:
							_robot->motor->SetGoToSpeedRampUp(value);
              break;
            case CMD_GOTO_SPEED_RAMP_DOWN:
							_robot->motor->SetGoToSpeedRampDown(value);
              break;
            case CMD_GOTO_SPEED_SCALING:
							value = _robot->motor->GetGoToSpeedScaling();
              break;
            default:
              DebugPrint("Unknown command");
              DebugPrint(_currentCommand);
          }
					if(value >= 0)
					{
						ReturnGetValue(_currentCommand, value);
					} else {
						////DEBUG_PRINT("Something wrong with geting speed");DEBUG_NL;
					}
     break;
    default:
      DEBUG_PRINT("SetGet fault");
  }
  
  _currentCommand = CMD_UNKNOWN;
  _currentSetGet = SETGET_UNKNOWN;
}

boolean KoppAPIParser::_getDataBytes()
{
  boolean result;
  result = Serial.readBytes(_dataByteBuffer, 2) == 2;
  return result;
}

int KoppAPIParser::_parseDataValue()
{
  int result;
  result = _dataByteBuffer[1] | (_dataByteBuffer[0] << 7);
  return result;
}

