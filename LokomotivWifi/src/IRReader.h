#ifndef IR_READER_H
#define IR_READER_H

#include "LokomotivWIFI.h"
#include "IRremote.h"

class Lokomotiv;

class IRReader {
public:
	IRReader(int receivePin, Lokomotiv *lok);
	void Init();
	void Update();
private:
	const static unsigned long kPad1;
	const static unsigned long kPad2;
	const static unsigned long kPad3;
	const static unsigned long kMagicNumber;
	int receivePin_;
	void resetParser_();
	IRrecv *receiver_;
	decode_results results_;
	unsigned long lastPadReceived_;
	int padsReceived_;
	unsigned long lastReceivedAddress_;
	Lokomotiv* lok_;
};
#include "Lokomotiv.h"

#endif
