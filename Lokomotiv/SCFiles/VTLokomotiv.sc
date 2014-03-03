VTLokomotiv {
	var port, <name;
	var <pid;
	var <speed;
	var <direction;
	var <targetPosition;
	var <lastDetectedAddress;
	var <measuredSpeed;
	var <distanceFromLastAddress;
	var <led;
	var <state;

	var parserState, parseRoutine, currentCommand, currentValueBytes;
	var currentStringData;
	classvar <command;
	classvar <setGet;
	classvar <byteType;
	classvar <valueType;

	*new{arg serialPort, name;
		^super.new.init(serialPort, name);
	}

	init{arg serialPort_, name_;
		port = serialPort_;
		name = name_;
		pid = (P: 0.0, I: 0.0, D: 0.0);
		currentStringData = String.new(128);
		currentValueBytes = Array.new(4);
		this.prResetParser;
		this.startParser;
		//poll values from robot
		//...
	}

	prResetParser{
		parserState = \waitingForCommandByte;
		currentCommand = nil;
	}

	startParser{
		parseRoutine = Routine({
			loop{
				this.prParseByte(port.read);
			}
		}).play;
	}

	stopParser{
		parseRoutine.stop;
		this.prResetParser;
	}

	close{
		port.close;
	}

	prParseByte{arg byte;
		switch(parserState,
			\waitingForCommandByte, {
				if(byte.bitAnd(0xC0) == 0x80, //check if it is command byte of 'set' type
					{
						//"Got command byte: %".format(this.class.command.getID(byte.bitAnd(0x0F))).postln;
						currentCommand = this.class.command.getID(byte.bitAnd(0x0F));//only read 4 low bits
						if(currentCommand == \info,
							{ parserState = \waitingForStringByte; },
							{ parserState = \waitingForValueByte; });
					},
					{
						this.prResetParser;
					}
				);
			},
			\waitingForValueByte, {
				if(byte != 143, //equals endTransmission set command byte
					{
						//"Got value byte: %".format(byte).postln;
						//byte is value byte
						currentValueBytes = currentValueBytes.add(byte);
					},
					{
						//byte marks end of transmission
						//"Got endTransmission byte: %".format(byte).postln;
						this.prProcessIncomingCommand(currentValueBytes.copy);
						currentValueBytes = Array.new(4);//reset data values
					}
				);
			},
			\waitingForStringByte, {
				if(byte != 4, //equals end of transmission byte acc. to ASCII
					{
						//"Got string value byte: %".format(byte.asAscii).postln;
						currentStringData = currentStringData.add(byte.asAscii);
					},
					{
						//"Got ASCII End of transmission".postln;
						this.prProcessIncomingCommand(currentStringData.copy);
						currentStringData = String.new(128);
					}
				)
			},
			{this.prResetParser};
		);
	}

	*parseDataBytes{arg bytes;
		var result;
		//the actual type is "hidden" in bit 6 of the last recieved value byte. If that is 1 it is a decimal number
		result = bytes.reverse.collect({arg it, i; it << (i * 7)}).reduce(\bitOr);
		if(bytes.first.bitAnd(0x40).booleanValue, {
			result = (result / 10000.0).asFloat;
		});
		^result;
	}

	//formats the received command and value bytes according to API
	prProcessIncomingCommand{arg dataBytes;
		switch(currentCommand,
			\speed, {speed = this.class.parseDataBytes(dataBytes); this.changed(\speed)},
			\stop, {this.changed(\stopped)},
			\direction, {direction = this.class.parseDataBytes(dataBytes); this.changed(\direction)},
			\targetPosition, {targetPosition = this.class.parseDataBytes(dataBytes); this.changed(\targetPosition)},
			\distanceFromLastAddress, {
				distanceFromLastAddress = this.class.parseDataBytes(dataBytes);
				this.changed(\distanceFromLastAddress)},
			\led, {led = this.class.parseDataBytes(dataBytes); this.changed(\led); },
			\state, {state = this.class.parseDataBytes(dataBytes); this.changed(\state);},
			\info, {"INFO: %".format(dataBytes).postln;},
			\measuredSpeed, {measuredSpeed = this.class.parseDataBytes(dataBytes); this.changed(\measuredSpeed);},
			\lastDetectedAddress, {lastDetectedAddress = this.class.parseDataBytes(dataBytes); this.changed(\lastDetectedAddress);},
			\pidPValue, {pid.put('P', this.class.parseDataBytes(dataBytes)); this.changed(\pid);},
			\pidIValue, {pid.put('I', this.class.parseDataBytes(dataBytes)); this.changed(\pid);},
			\pidDValue, {pid.put('D', this.class.parseDataBytes(dataBytes)); this.changed(\pid);}
		);
		this.prResetParser;
	}

	*encodeValue{arg value;
		var result, valueToSend, typeCode;
		result = Array.new(5);
		//this method returns an array of bytes. The class of the value
		//determines the encoding type.
		if(value.isNumber, {
			if(value.isFloat, {
				valueToSend = (value.clip(-214748, 214748) * 10000).asInteger;
				typeCode = this.valueType[\decimal];
			});
			if(value.isInteger,{
				valueToSend = value;
				typeCode = this.valueType[\integer];
			});
			}, {
				^nil;
		});
		result = result.addAll(
			[
				((valueToSend & 0xF0000000) >> 28).bitAnd(0x0F).bitOr(typeCode),
				(valueToSend & 0x0FE00000) >> 21,
				(valueToSend & 0x001FC000) >> 14,
				(valueToSend & 0x00003F80) >> 7,
				(valueToSend & 0x0000007F)
			];
		);
		^result.as(Int8Array);
	}

	set{arg command, value;
		var msg = Array.new(7);
		msg = msg.add(
			this.class.command[command.asSymbol] ?? {"uknown command".error.postln; ^this;}|
			this.class.setGet[\set] |
			this.class.byteType[\command]
		);
		msg = msg.addAll(this.class.encodeValue(value));
		msg = msg.add(
			this.class.command[\endTransmission] |
			this.class.setGet[\set] |
			this.class.byteType[\command]
		);
		//		msg = msg.as(Int8Array);
		port.putAll(msg);
		"Sending mess: %".format(msg).postln;

	}

	get{arg command;
		//find command code and add command byte bit
		if(command == \pid, {
			fork{
				[\pidPValue, \pidIValue, \pidDValue].do({arg item;
					port.put(
						this.class.command[item] |
						this.class.setGet[\get] |
						this.class.byteType[\command]
					);
				});
				0.001.wait;
			};
			^this;
		});
		port.put(
			this.class.command[command.asSymbol] |
			this.class.setGet[\get] |
			this.class.byteType[\command]
		);
	}

	prSend{arg msg;
		msg = msg.asString.ascii.as(Int8Array);
		port.putAll(msg);
	}

	*initClass{
		command = TwoWayIdentityDictionary[
			\stop -> 0x00,
			\speed -> 0x01,
			\direction -> 0x02,
			\targetPosition -> 0x03,
			\distanceFromLastAddress -> 0x04,
			\led -> 0x05,
			\state -> 0x06,
			\info -> 0x07,
			\measuredSpeed -> 0x08,
			\lastDetectedAddress -> 0x09,
			\pidPValue -> 0x0A,
			\pidIValue -> 0x0B,
			\pidDValue -> 0x0C,
			\endTransmission -> 0x0F
		];
		setGet = TwoWayIdentityDictionary[
			\set -> 0x00,
			\get -> 0x40
		];
		byteType = TwoWayIdentityDictionary[
			\command -> 0x80,
			\value -> 0x00
		];
		valueType = TwoWayIdentityDictionary[
			\integer -> 0x00,
			\decimal -> 0x40
		];
	}
}
