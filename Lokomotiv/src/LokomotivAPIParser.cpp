#include "LokomotivAPIParser.h"

LokomotivAPIParser::LokomotivAPIParser(Lokomotiv* robot) :
  _robot(robot),
  _currentCommand(CMD_UNKNOWN),
  _currentSetGet(SETGET_UNKNOWN),
	_currentDataType(TYPE_UNKNOWN),
	_state(WAITING_FOR_COMMAND_BYTE),
	_data(0),
	_numValueBytesReceived(0)
{
}

void LokomotivAPIParser::parseIncomingByte(byte inByte)
{
	switch(_state)
	{
		case WAITING_FOR_COMMAND_BYTE:
			_currentCommand = (command_t)(inByte & PARSE_MASK_COMMAND);
			_currentSetGet = (setGet_t)(inByte & PARSE_MASK_SETGET);
			if(_currentSetGet == SET_MESSAGE)
			{
				_state = WAITING_FOR_VALUE_BYTE;
			} else {
				_executeCommand();
			}
			break;
		case WAITING_FOR_VALUE_BYTE:
			_parseValueByte(inByte);
			break;
		default:
			_reset();
	}
}

void LokomotivAPIParser::_reset()
{
	_state = WAITING_FOR_COMMAND_BYTE;
	_numValueBytesReceived = 0;
	_data = 0;
	_currentDataType = TYPE_UNKNOWN;
	_currentCommand = CMD_UNKNOWN;
	_currentSetGet = SETGET_UNKNOWN;
}

void LokomotivAPIParser::_parseValueByte(byte inByte)
{
	//check if first bit is not 1
	if(!(inByte & 0x80))
	{
		switch(_numValueBytesReceived)
		{
			case 0:
				_currentDataType = (dataTypeMark_t)(inByte & PARSE_MASK_DATA_TYPE);
				_data |= static_cast<uint32_t>(inByte & 0x0F) << 28;
				break;
			case 1:
				_data |= static_cast<uint32_t>(inByte) << 21;
				break;
			case 2:
				_data |= static_cast<uint32_t>(inByte) << 14;
				break;
			case 3:
				_data |= static_cast<uint32_t>(inByte) << 7;
				break;
			case 4:
				_data |= static_cast<uint32_t>(inByte);
				break;
			default:
				_reset();
		}
		_numValueBytesReceived++;	
	} else if(inByte == (CMD_END_TRANSMISSION | BYTE_COMMAND | SET_MESSAGE)) {
		_executeCommand();
		_reset();
	} else {
		_reset();
	}
}

long LokomotivAPIParser::_decodeIntegerValue(uint32_t data)
{
	long result;
	result = *(reinterpret_cast<long*>(&data));
	return result;
}

double LokomotivAPIParser::_decodeDecimalValue(uint32_t data)
{
	double result;
	long rawNum = _decodeIntegerValue(data);
	result = static_cast<double>(rawNum) / 10000.0; 
	return result;
}

void LokomotivAPIParser::_executeCommand()
{
  switch(_currentSetGet)
  {
    case SET_MESSAGE:
			switch(_currentCommand)
			{
				case CMD_STOP:
					_robot->Stop();
					break;
				case CMD_SPEED:
					_robot->SetSpeed(_decodeIntegerValue(_data));
					break;
				case CMD_DIRECTION:
					_robot->SetDirection(_decodeIntegerValue(_data));
					break;
				case CMD_TARGET_POSITION:
					_robot->SetTargetPosition(_decodeIntegerValue(_data));
					break;
				case CMD_DISTANCE_FROM_LAST_ADDRESS:
					//will remove after implementation is done
					_robot->SetDistanceFromLastAddress(_decodeIntegerValue(_data));
					break;
				case CMD_LED:
					_robot->SetLed(_decodeIntegerValue(_data));
					break;
				case CMD_STATE:
					//will remove after implementation is done
					_robot->SetState(_decodeIntegerValue(_data));
					break;
				case CMD_INFO:
					break;
				case CMD_MEASURED_SPEED:
					//will remove after implementation is done
					_robot->SetMeasuredSpeed(_decodeIntegerValue(_data));
					break;
				case CMD_LAST_DETECTED_ADDRESS:
					//will remove after implementation is done
					_robot->SetLastDetectedAddress(_decodeIntegerValue(_data));
					break;
				case CMD_PID_P_VALUE:
					_robot->SetPidPValue(_decodeDecimalValue(_data));
					break;
				case CMD_PID_I_VALUE:
					_robot->SetPidIValue(_decodeDecimalValue(_data));
					break;
				case CMD_PID_D_VALUE:
					_robot->SetPidDValue(_decodeDecimalValue(_data));
					break;
				default:
					DebugPrint("Unknown command");
					DebugPrint(_currentCommand);
			}
			break;
    case GET_MESSAGE:
      switch(_currentCommand) 
			{
				case CMD_STOP:
					break;
				case CMD_SPEED:
					ReturnGetValue(CMD_SPEED,_robot->GetSpeed());
					break;
				case CMD_DIRECTION:
					ReturnGetValue(CMD_DIRECTION,_robot->GetDirection());
					break;
				case CMD_TARGET_POSITION:
					ReturnGetValue(CMD_TARGET_POSITION,_robot->GetTargetPosition());
					break;
				case CMD_DISTANCE_FROM_LAST_ADDRESS:
					ReturnGetValue(CMD_DISTANCE_FROM_LAST_ADDRESS,_robot->GetDistanceFromLastAddress());
					break;
				case CMD_LED:
					ReturnGetValue(CMD_LED,_robot->GetLed());
					break;
				case CMD_STATE:
					ReturnGetValue(CMD_STATE,_robot->GetState());
					break;
				case CMD_INFO:
					break;
				case CMD_MEASURED_SPEED:
					ReturnGetValue(CMD_MEASURED_SPEED,_robot->GetMeasuredSpeed());
					break;
				case CMD_LAST_DETECTED_ADDRESS:
					ReturnGetValue(CMD_LAST_DETECTED_ADDRESS,_robot->GetLastDetectedAddress());
					break;
				case CMD_PID_P_VALUE:
					ReturnGetValue(CMD_PID_P_VALUE,_robot->GetPidPValue());
					break;
				case CMD_PID_I_VALUE:
					ReturnGetValue(CMD_PID_I_VALUE,_robot->GetPidIValue());
					break;
				case CMD_PID_D_VALUE:
					ReturnGetValue(CMD_PID_D_VALUE,_robot->GetPidDValue());
					break;
				default:
					DebugPrint("Unknown command");
					DebugPrint(_currentCommand);
      }
      break;
    default:
      DEBUG_PRINT("SetGet fault");
  }
  
  _currentCommand = CMD_UNKNOWN;
  _currentSetGet = SETGET_UNKNOWN;
}

