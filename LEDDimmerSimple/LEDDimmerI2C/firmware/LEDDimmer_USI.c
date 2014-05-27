#include "LEDDimmer_USI.h"

uint8_t address;
uint8_t commandQueue[I2C_BUFFER_SIZE];
uint8_t commandQueueHead;
uint8_t commandQueueTail;

uint16_t dataQueue[I2C_BUFFER_SIZE];

volatile uint8_t currentCommand;
volatile uint16_t currentData;

enum
{
  I2C_SLAVE_WAITING_FOR_ADDRESS_BYTE,
	I2C_SLAVE_WAITING_FOR_COMMAND_BYTE,
	I2C_SLAVE_WAITING_FOR_DATA_BYTE0,
	I2C_SLAVE_WAITING_FOR_DATA_BYTE1,
} USIOverflowParserState;

void InitUSI(uint8_t addr)
{
	// Set SCL high
	PORT_USI |= (1<<PORT_USI_SCL);
	// Set SDA high
	PORT_USI |= (1<<PORT_USI_SDA);
	// Set SCL as output
	USI_SET_SCL_OUTPUT();
	// Set SDA as input
	USI_SET_SDA_INPUT();

	address = addr;
	
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
	FlushI2CBuffers();
}

void SetI2CAddress(uint8_t val)
{
	address = val;
}

void FlushI2CBuffers()
{
	commandQueueHead = 0;
	commandQueueTail = 0;
	dataQueueHead = 0;
	dataQueueTail = 0;
}

uint8_t HasQueuedCommands()
{
	return !(commandQueueHead == commandQueueTail);
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
	USIOverflowParserState = I2C_SLAVE_WAITING_FOR_ADDRESS_BYTE;
	//wait until SCL line goes low to ensure that a full start condition
	//has been met. Both SDA and SCL must be pulled low for that to happen.
	while((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA))));	
	//if the SDA pin is high again it means a stop condition
	//has been met, i.e. SDA pulled high while SCK is high.
	//If that is the case we restart the parsing process.
	if(PORT_USI & (1<<PIN_USI_SDA))
	{
		USICR = USICR_PREPARE_FOR_RECEIVING_START_CONDITION;
	} else {
		USICR = USICR_PREPARE_FOR_RECEIVING_DATA_BYTES;
	}
	USISR = USISR_CLEAR_COUNTER_AND_FLAGS;
}

ISR(USI_OVF_vect)
{
	switch(USIOverflowParserState)
	{
		case I2C_SLAVE_WAITING_FOR_ADDRESS_BYTE:
			//check if address is a match
			if((USIDR == 0) || (USIDR >> 1) == address)
			{
				//check r/w bit
				if(USIDR & 0x01)
				{
					//USIOverflowParserState = I2C_SLAVE_SEND_DATA;
				} else {
					USIOverflowParserState = I2C_SLAVE_WAITING_FOR_COMMAND_BYTE;
					SET_USI_TO_SEND_ACK();
				}
			}
			break;
		case I2C_SLAVE_WAITING_FOR_COMMAND_BYTE:
			commandQueue[++commandQueueHead] = USIDR;
			USIOverflowParserState = I2C_SLAVE_WAITING_FOR_DATA_BYTE0;
			SET_USI_TO_SEND_ACK();
			break;
		case I2C_SLAVE_WAITING_FOR_DATA_BYTE0:
			dataQueue[commandQueueHead] = USIDR;
			USIOverflowParserState = I2C_SLAVE_WAITING_FOR_DATA_BYTE1;
			SET_USI_TO_SEND_ACK();
			break;
		case I2C_SLAVE_WAITING_FOR_DATA_BYTE1:
			dataQueue[commandQueueHead] |= ((uint16_t)USIDR) << 8;
			USIOverflowParserState = I2C_SLAVE_WAITING_FOR_ADDRESS_BYTE;
			SET_USI_TO_SEND_ACK();
			break;


	}
}

