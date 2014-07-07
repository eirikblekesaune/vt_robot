#include "LEDDimmer_USI.h"

uint8_t address;

volatile uint8_t currentCommand;
volatile uint8_t currentSetGet;
volatile uint8_t currentDataByte0;
volatile uint8_t currentDataByte1;

uint8_t commandQueue[TWI_BUFFER_SIZE];
uint16_t commandDataQueue[TWI_BUFFER_SIZE];
volatile uint8_t commandQueueHead;
volatile uint8_t commandQueueTail;

uint8_t transmitQueue[TWI_BUFFER_SIZE];
uint16_t transmitDataQueue[TWI_BUFFER_SIZE];
volatile uint8_t transmitQueueHead;
volatile uint8_t transmitQueueTail;

volatile uint8_t numBytesReceived = 0;
volatile uint8_t numBytesTransmitted = 0;

//Function defined as macro to be used i ISR routine
#define PUSH_DATA_TO_COMMAND_QUEUE()\
{\
	commandQueueHead++;\
	commandQueue[commandQueueHead] = currentCommand;\
	commandDataQueue[commandQueueHead] = ((uint16_t)currentDataByte1 << 8) | currentDataByte0;\
}\

///The GetNextCommand and GetDataAtTail functions are to
//be called in conjuction since the first function increases
//the tail pointer in the queue by one.
uint8_t GetNextCommand()
{
	commandQueueTail++;
	return commandQueue[commandQueueTail];
}

uint16_t GetDataAtTail()
{
	return commandDataQueue[commandQueueTail];
}

uint8_t HasQueuedCommands()
{
	return commandQueueHead != commandQueueTail;
}

enum
{
  USI_OVF_WAITING_FOR_ADDRESS_BYTE,
	USI_OVF_SENDING_ACK,
	USI_OVF_WAITING_FOR_COMMAND_BYTE,
	USI_OVF_WAITING_FOR_DATA_BYTE0,
	USI_OVF_WAITING_FOR_DATA_BYTE1,
	USI_OVF_SENDING_ACK_FOR_COMMAND_REQUEST,
	USI_OVF_WAITING_FOR_ACK,
	USI_OVF_WAITING_FOR_REQUESTED_COMMAND_BYTE,
	USI_OVF_SENDING_COMMAND_BYTE,
	USI_OVF_SENDING_DATA_BYTE0,
	USI_OVF_SENDING_DATA_BYTE1,
} USIOverflowParserState;

void InitUSI(uint8_t addr)
{
	FlushTWIBuffers();

	PORT_USI |= (1<<PORT_USI_SCL);// Set SCL high
	PORT_USI |= (1<<PORT_USI_SDA);// Set SDA high
	USI_SET_SCL_OUTPUT();// Set SCL as output
	USI_SET_SDA_INPUT();// Set SDA as input

	SetTWIAddress(addr);
	
	USICR = (1 << USISIE) | //Enable start condition interrupt 
					(0 << USIOIE) | //Overflow interrupt is disabled
					(1 << USIWM1) | //Wire mode 1:0 = b10 Two-wire mode
				 	(0 << USIWM0) | 
					(1 << USICS1) | //External clock source, positive edge
					(0 << USICS0) | 
					(0 << USICLK) | 
					(0 << USITC);
	USISR = (1 << USISIF) | //clear start condition interrupt flag
					(1 << USIOIF) | //clear counter overflow interupt flag
					(1 << USIPF) | //clear stop condition flag
					(1 << USIDC); //clear data collistion flag
	SET_USI_TO_RECEIVE_START_CONDITION_MODE();
}

void SetTWIAddress(uint8_t val)
{
	address = val;
}

void FlushTWIBuffers()
{
	commandQueueHead = 0;
	commandQueueTail = 0;
	transmitQueueHead = 0;
	transmitQueueTail = 0;

}



//Disable counter interrupt, enable start condition interrupt, 
//TWI mode with low-hold SCL on counter overflow
#define USICR_PREPARE_FOR_RECEIVING_START_CONDITION 0b10111000
//Enable counter overflow interrupt. TWI mode without low-hold 
//on counter overflow.
#define USICR_PREPARE_FOR_RECEIVING_DATA_BYTES 0b11101000

#define USISR_CLEAR_COUNTER_AND_FLAGS 0b11110000 

//Start condition interrupt routine
ISR(USI_STR_vect)
{
	OCR1B = 0x0000;
	USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
	//wait until SCL line goes low to ensure that a full start condition
	//has been met. Both SDA and SCL must be pulled low for that to happen.
	while((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA))));	
	//if the SDA pin is high again it means a stop condition
	//has been met, i.e. SDA pulled high while SCK is high.
	//If that is the case we restart the parsing process.
	if(PORT_USI & (1<<PIN_USI_SDA))
	{
		SET_USI_TO_RECEIVE_START_CONDITION_MODE();
	} else {
		SET_USI_TO_RECEIVE_DATA_BYTES_MODE();
	}
	USISR = USISR_CLEAR_COUNTER_AND_FLAGS;
}

ISR(USI_OVF_vect)
{
	switch(USIOverflowParserState)
	{
		case USI_OVF_WAITING_FOR_ADDRESS_BYTE:
			//check if address is a match, or if global broadcast message
			if((USIDR == 0) || (USIDR >> 1) == address)
			{
				//check r/w bit, 1 if master request 
				if(USIDR & 0x01)
				{
					USIOverflowParserState = USI_OVF_SENDING_ACK_FOR_COMMAND_REQUEST;
					SET_USI_TO_SEND_ACK();
				} else {
					USIOverflowParserState = USI_OVF_SENDING_ACK;
					SET_USI_TO_SEND_ACK();
					numBytesReceived = 0;
				}
			} else {
				SET_USI_TO_RECEIVE_START_CONDITION_MODE();
			}
			break;
		case USI_OVF_SENDING_ACK:
			switch(numBytesReceived)
			{
				case 0:
					USIOverflowParserState = USI_OVF_WAITING_FOR_COMMAND_BYTE;
					numBytesReceived++;
					SET_USI_TO_RECEIVE_DATA_BYTES_MODE();
					break;
				case 1:
					USIOverflowParserState = USI_OVF_WAITING_FOR_DATA_BYTE0;
					numBytesReceived++;
					SET_USI_TO_RECEIVE_DATA_BYTES_MODE();
					break;
				case 2:
					USIOverflowParserState = USI_OVF_WAITING_FOR_DATA_BYTE1;
					numBytesReceived++;
					SET_USI_TO_RECEIVE_DATA_BYTES_MODE();
					break;
				case 3:
					USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
					numBytesReceived++;
					SET_USI_TO_RECEIVE_START_CONDITION_MODE();
					break;
				default:
					USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
					SET_USI_TO_RECEIVE_START_CONDITION_MODE();
			}
			break;
		case USI_OVF_WAITING_FOR_COMMAND_BYTE:
			currentCommand = USIDR;
			USIOverflowParserState = USI_OVF_SENDING_ACK;
			SET_USI_TO_SEND_ACK();
			break;
		case USI_OVF_WAITING_FOR_DATA_BYTE0:
			currentDataByte0 = USIDR;
			USIOverflowParserState = USI_OVF_SENDING_ACK;
			SET_USI_TO_SEND_ACK();
			break;
		case USI_OVF_WAITING_FOR_DATA_BYTE1:
			currentDataByte1 = USIDR;
			USIOverflowParserState = USI_OVF_SENDING_ACK;
			PUSH_DATA_TO_COMMAND_QUEUE();
			SET_USI_TO_SEND_ACK();
			break;
		case USI_OVF_SENDING_ACK_FOR_COMMAND_REQUEST:
			USIOverflowParserState = USI_OVF_WAITING_FOR_REQUESTED_COMMAND_BYTE;
			break;
		case USI_OVF_WAITING_FOR_ACK:
			//if the incoming data is not 0 it means that an ACK has not
			//been sent from the master. Parser will then be reset.
			if(USIDR)
			{
				USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
				SET_USI_TO_RECEIVE_START_CONDITION_MODE();
			} else {
				switch(numBytesTransmitted)
				{
					case 0:
						USIOverflowParserState = USI_OVF_SENDING_COMMAND_BYTE;
						break;
					case 1:
						USIOverflowParserState = USI_OVF_SENDING_DATA_BYTE0;
						break;
					case 2:
						USIOverflowParserState = USI_OVF_SENDING_DATA_BYTE1;
						break;
					case 3:
						USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
						break;
					default:
						USIOverflowParserState = USI_OVF_WAITING_FOR_ADDRESS_BYTE;
						break;
				}
			}
			break;
		case USI_OVF_WAITING_FOR_REQUESTED_COMMAND_BYTE:
			currentCommand = USIDR;
			USIOverflowParserState = USI_OVF_SENDING_ACK;
			SET_USI_TO_SEND_ACK();
			break;
		case USI_OVF_SENDING_COMMAND_BYTE:
			currentCommand = 0x00;//here the command keys is loaded for sending
			USIDR = currentCommand;
			USIOverflowParserState = USI_OVF_WAITING_FOR_ACK;
			break;
		case USI_OVF_SENDING_DATA_BYTE0:
			currentDataByte0 = 0x01;
			USIDR = currentDataByte0;
			USIOverflowParserState = USI_OVF_WAITING_FOR_ACK;
			break;
		case USI_OVF_SENDING_DATA_BYTE1:
			currentDataByte1 = 0x02;
			USIDR = currentDataByte1;
			USIOverflowParserState = USI_OVF_WAITING_FOR_ACK;
			break;
	}
}

