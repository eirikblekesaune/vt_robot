#include "IRReader.h"

const unsigned long IRReader::kPad1 = 0x11111111;
const unsigned long IRReader::kPad2 = 0x22222222;
const unsigned long IRReader::kPad3 = 0x33333333;
const unsigned long IRReader::kMagicNumber = 0xABCDEF00;

IRReader::IRReader(int receivePin, Lokomotiv *lok) :
	receivePin_(receivePin),
	padsReceived_(0),
	lastPadReceived_(0),
	lok_(lok)
{
	receiver_ = new IRrecv(receivePin);
}

void IRReader::Init()
{
	receiver_->enableIRIn();
}

void IRReader::Update()
{
	if(receiver_->decode(&results_))
	{
		unsigned long result = results_.value;
		DebugPrint("deconding");
		if(results_.decode_type == NEC)
		{
			if(result == kPad1)
			{
				lastPadReceived_ = result;
				padsReceived_++;
			} else if(result == kPad2)
			{
				if(lastPadReceived_ == kPad1)
				{
					lastPadReceived_ = result;
					padsReceived_++;
				} else {
					resetParser_();
				}
			} else if(result == kPad3)
			{
				if(lastPadReceived_ == kPad2)
				{
					lastPadReceived_ = result;
					padsReceived_++;
				} else {
					resetParser_();
				}
			} else if(padsReceived_ == 3 && (lastPadReceived_ == kPad3))
			{
				if((result & 0xFFFFFF00) == kMagicNumber)
				{
					lok_->GotAddr(static_cast<unsigned char>(result & 0x000000FF));
					resetParser_();
				}
			}
		} else {
			resetParser_();
		} 
		receiver_->resume();
	}
}

void IRReader::resetParser_()
{
	padsReceived_ = 0;
	lastPadReceived_ = 0;
}
