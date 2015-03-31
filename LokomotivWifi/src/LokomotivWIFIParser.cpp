#include "LokomotivWIFIParser.h"

//
//
LokomotivWIFIParser::LokomotivWIFIParser(Lokomotiv* robot) :
	_robot(robot),
	_currentCommand(COMMAND_UNKNOWN),
	_currentSetGet(SETGET_UNKNOWN),
	_currentTypeTag(TYPE_UNKNOWN),
	_currentLength(0),
	_currentIP(0),
	_currentDestinationPort(0),
	_currentSourcePort(0),
	_currentExpectedDataLength(0),
	_state(XBEE_CHECK_START_DELIMITER),
	_value(0),
	_numValueBytesReceived(0)
{
}

void LokomotivWIFIParser::parseIncomingByte(int inByte)
{
	static uint16_t byteCounter = 0;//used for counting bytes in the state machine
	//SendMsg(COMMAND_INFO, _state);
	switch(_state)
	{
		case XBEE_CHECK_START_DELIMITER:
			if(inByte == kXBeeStartDelimiter)
			{
				_state = XBEE_READ_LENGTH;
				byteCounter = 0; //init byteCounter to count length bytes
			} else { _reset(); }
			break;
		case XBEE_READ_LENGTH:
			//length comes in two bytes
			if(byteCounter == 0) {
				_currentLength = static_cast<uint16_t>(inByte) << 8;
				byteCounter = 1;
			} else if (byteCounter == 1) {
				_currentLength |= static_cast<uint16_t>(inByte);
				_state = XBEE_CHECK_API_FRAME_ID;
			} else {
				_reset();
			}
			break;
		case XBEE_CHECK_API_FRAME_ID:
			if(inByte == kXBeeReceivePacketFrame) //we only care about receive packets
			{
				_state = XBEE_READ_SOURCE_IP;
				byteCounter = 0;
			} else { _reset(); }
			break;
		case XBEE_READ_SOURCE_IP:
			if(byteCounter < 4) {
				if(byteCounter == 0) {
					_currentIP |= static_cast<uint32_t>(inByte) << 24;
				} else if(byteCounter == 1) {
					_currentIP |= static_cast<uint32_t>(inByte) << 16;
				} else if(byteCounter == 2) {
					_currentIP |= static_cast<uint32_t>(inByte) << 8;
				} else if(byteCounter == 3) {
					_currentIP |= static_cast<uint32_t>(inByte);
				}
				byteCounter++;

				if(byteCounter == 4) {
					_state = XBEE_READ_DESTINATION_PORT;
					byteCounter = 0;
				}
			} else {
				_reset();//should never happen
				SendMsg(COMMAND_PARSER_ERROR, "EXCEPTION2");
			}
			break;
		case XBEE_READ_DESTINATION_PORT:
			//destination port is sent over two bytes
			if(byteCounter == 0) {
				_currentDestinationPort = static_cast<uint16_t>(inByte) << 8;
				byteCounter = 1;
			} else if (byteCounter == 1) {
				_currentDestinationPort |= static_cast<uint16_t>(inByte);
				_state = XBEE_READ_SOURCE_PORT;
				byteCounter = 0;
			} else {
				_reset();
			}
			break;
		case XBEE_READ_SOURCE_PORT:
			//source port is sent over two bytes
			if(byteCounter == 0) {
				_currentSourcePort = static_cast<uint16_t>(inByte) << 8;
				byteCounter = 1;
			} else if (byteCounter == 1) {
				_currentSourcePort |= static_cast<uint16_t>(inByte);
				_state = XBEE_CHECK_PROTOCOL;
				byteCounter = 0;
			} else {
				_reset();
			}
			break;
		case XBEE_CHECK_PROTOCOL:
			if(inByte == 0) {//Should be 0x00 for UDP
				_state = XBEE_CHECK_STATUS;
			} else {
				_reset();
			}
			break;
		case XBEE_CHECK_STATUS:
			if(inByte == 0) {
				_state = XBEE_READ_OSC_DATA;
				byteCounter = 0;
				_currentExpectedDataLength = _currentLength - 11;
			} else {
				_reset();
			}
			break;
		case XBEE_READ_OSC_DATA:
			if(byteCounter < _currentExpectedDataLength) {
				_data[byteCounter] = inByte;
				byteCounter++;
				if(byteCounter == _currentExpectedDataLength) {
					_state = XBEE_READ_CHECKSUM;
				}
			}
			break;
		case XBEE_READ_CHECKSUM:
			//dont care about doing checksumum for now
			if(_parseOSCData()) {
				SendMsg(COMMAND_PARSER_ERROR, "ParseRRROR");
			}
			_reset();
			break;
		default:
			_reset();
	}
}

oscDataParseError_t LokomotivWIFIParser::_parseOSCData() {//Should return error type
	uint16_t i;

	//check leading slash
	if(_data[0] != '/')
		return PARSE_FAILED_NO_LEADING_SLASH;
	if(_data[1] != 't')
		return PARSE_FAILED_NO_T_CHAR;
	if(_data[2] != '/')
		return PARSE_FAILED_NO_SECOND_SLASH;
	_currentCommand = static_cast<command_t>(_data[3]);
	_currentSetGet = static_cast<setGet_t>(_data[4]);
	if((_data[5] != 0) && (_data[6] != 0) && (_data[7] != 0))
		return PARSE_FAILED;
	if(_data[8] != ',')
		return PARSE_FAILED;
	_currentTypeTag = static_cast<oscTypeTag_t>(_data[9]);
	if((_data[10] != 0) && (_data[11] != 0))
		return PARSE_FAILED;
	_value = static_cast<uint32_t>(_data[12]) << 24;
	_value |= static_cast<uint32_t>(_data[13]) << 16;
	_value |= static_cast<uint32_t>(_data[14]) << 8;
	_value |= static_cast<uint32_t>(_data[15]);

//	switch(_currentTypeTag) {
//		case 'i':
//		case 'f':
//			_executeCommand();
//			break;
//		default:
//			return PARSE_FAILED_WRONG_TYPETAG;
//	}
	_executeCommand();
	return PARSE_OK;
}

void LokomotivWIFIParser::_reset()
{
	_state = XBEE_CHECK_START_DELIMITER;
	_numValueBytesReceived = 0;
	_value = 0;
	_currentTypeTag = TYPE_UNKNOWN;
	_currentCommand = COMMAND_UNKNOWN;
	_currentSetGet = SETGET_UNKNOWN;
	_currentLength = 0;
	_currentIP = 0;
}

int32_t LokomotivWIFIParser::_decodeIntegerValue(uint32_t val) {
	return *(reinterpret_cast<int32_t *>(&val));
}

double LokomotivWIFIParser::_decodeDecimalValue(uint32_t val) {
	return *(reinterpret_cast<double *>(&val));
}


void LokomotivWIFIParser::_executeCommand()
{
//	SendMsg(COMMAND_INFO, "command");
//	SendMsg(COMMAND_INFO, _currentCommand);
//	SendMsg(COMMAND_INFO, "setget");
//	SendMsg(COMMAND_INFO, _currentSetGet);
//	SendMsg(COMMAND_INFO, "tag");
//	SendMsg(COMMAND_INFO, _currentTypeTag);
//	SendMsg(COMMAND_INFO, "val");
//	switch(_currentTypeTag) {
//		case 'i':
//			SendMsg(COMMAND_INFO, *(reinterpret_cast<int32_t*>(&_value)));
//			break;
//		case 'f':
//			SendMsg(COMMAND_INFO, *(reinterpret_cast<double*>(&_value)));
//			break;
//	}
	
	switch(_currentSetGet)
	{
		case SET_MESSAGE:
		switch(_currentCommand)
		{
		case COMMAND_STOP:
			_robot->Stop(_decodeIntegerValue(_value));
			break;
		case COMMAND_SPEED:
			_robot->SetSpeed(_decodeIntegerValue(_value));
			_robot->UserChangedSpeed();
			break;
		case COMMAND_DIRECTION:
			_robot->SetDirection(_decodeIntegerValue(_value));
			break;
		case COMMAND_TARGET_POSITION:
			_robot->SetTargetPosition(_decodeIntegerValue(_value));
			break;
		case COMMAND_DISTANCE_FROM_LAST_ADDRESS:
			_robot->SetDistanceFromLastAddress(_decodeIntegerValue(_value));
			break;
		case COMMAND_PERIPHERAL:
			_robot->SetPeripheral(_decodeIntegerValue(_value));
			break;
		case COMMAND_PERIPHERAL_REQUEST:
			_robot->SetPeripheralRequest(_decodeIntegerValue(_value));
			break;
		case COMMAND_STATE:
			break;
		case COMMAND_INFO:
			break;
		case COMMAND_MEASURED_SPEED:
			break;
		case COMMAND_LAST_DETECTED_ADDRESS:
			break;
		case COMMAND_PID_P_VALUE:
			_robot->SetPidPValue(_decodeDecimalValue(_value));
			break;
		case COMMAND_PID_I_VALUE:
			_robot->SetPidIValue(_decodeDecimalValue(_value));
			break;
		case COMMAND_PID_D_VALUE:
			_robot->SetPidDValue(_decodeDecimalValue(_value));
			break;
		case COMMAND_GLIDE_TO_SPEED:
			_robot->SetGlideToSpeed(_decodeIntegerValue(_value));
			break;
		case COMMAND_END_SPEED:
			_robot->SetEndSpeed(_decodeIntegerValue(_value));
			break;
		case COMMAND_MOTOR_MODE:
			_robot->SetMotorMode(_decodeIntegerValue(_value));
			break;
		case COMMAND_TRACKING_POLLING_INTERVAL:
			_robot->SetTrackingPollingInterval(_decodeIntegerValue(_value));
			break;
		case COMMAND_PID_TARGET_SPEED:
			_robot->SetPidTargetSpeed(_decodeDecimalValue(_value));
			break;
		case COMMAND_TRACKING_STATE:
			_robot->SetTrackingState(_decodeIntegerValue(_value));
			break;
		case COMMAND_WHOAREYOU:
		case COMMAND_PARSER_ERROR:
			break;
		default:
			DebugPrint("Unknown command");
			DebugPrint(_currentCommand);
		}
		break;
		case GET_MESSAGE:
			switch(_currentCommand)
		{
		case COMMAND_STOP:
			break;
		case COMMAND_SPEED:
			SendMsg(COMMAND_SPEED,_robot->GetSpeed());
			break;
		case COMMAND_DIRECTION:
			SendMsg(COMMAND_DIRECTION,_robot->GetDirection());
			break;
		case COMMAND_TARGET_POSITION:
			SendMsg(COMMAND_TARGET_POSITION,_robot->GetTargetPosition());
			break;
		case COMMAND_DISTANCE_FROM_LAST_ADDRESS:
			SendMsg(COMMAND_DISTANCE_FROM_LAST_ADDRESS,_robot->GetDistanceFromLastAddress());
			break;
		case COMMAND_PERIPHERAL:
			SendMsg(COMMAND_PERIPHERAL,_robot->GetPeripheral(0));
			break;
		case COMMAND_PERIPHERAL_REQUEST:
			break;
		case COMMAND_STATE:
			SendMsg(COMMAND_STATE,_robot->GetState());
			break;
		case COMMAND_INFO:
			break;
		case COMMAND_MEASURED_SPEED:
			SendMsg(COMMAND_MEASURED_SPEED,_robot->GetMeasuredSpeed());
			break;
		case COMMAND_LAST_DETECTED_ADDRESS:
			SendMsg(COMMAND_LAST_DETECTED_ADDRESS,_robot->GetLastDetectedAddress());
			break;
		case COMMAND_PID_P_VALUE:
			SendMsg(COMMAND_PID_P_VALUE,_robot->GetPidPValue());
			break;
		case COMMAND_PID_I_VALUE:
			SendMsg(COMMAND_PID_I_VALUE,_robot->GetPidIValue());
			break;
		case COMMAND_PID_D_VALUE:
			SendMsg(COMMAND_PID_D_VALUE,_robot->GetPidDValue());
			break;
		case COMMAND_GLIDE_TO_SPEED:
			break;
		case COMMAND_END_SPEED:
			SendMsg(COMMAND_END_SPEED,_robot->GetEndSpeed());
			break;
		case COMMAND_MOTOR_MODE:
			SendMsg(COMMAND_MOTOR_MODE,_robot->GetMotorMode());
			break;
		case COMMAND_TRACKING_POLLING_INTERVAL:
			SendMsg(COMMAND_TRACKING_POLLING_INTERVAL,_robot->GetTrackingPollingInterval());
			break;
		case COMMAND_PID_TARGET_SPEED:
			SendMsg(COMMAND_PID_TARGET_SPEED, _robot->GetPidTargetSpeed());
			break;
		case COMMAND_TRACKING_DATA:
			//SendMsg(COMMAND_TRACKING_DATA, _robot->GetTrackingData());
			break;
		case COMMAND_TRACKING_STATE:
			SendMsg(COMMAND_TRACKING_STATE, _robot->GetTrackingState());
			break;
		case COMMAND_WHOAREYOU:
			//SendMsg(COMMAND_WHOAREYOU, _robot->GetName());
			//SendMsg(COMMAND_WHOAREYOU, "figur3");
			WhoAreYou();
			break;
		case COMMAND_PARSER_ERROR:
			break;
		default:
			DebugPrint("Unknown command");
			DebugPrint(_currentCommand);
			}
			break;
	}

	_reset();
}

